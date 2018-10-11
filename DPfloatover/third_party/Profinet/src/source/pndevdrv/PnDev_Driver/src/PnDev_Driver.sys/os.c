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
/*  F i l e               &F: os.c                                      :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//  Code for adaption of SYS to used OS
//
//****************************************************************************/
//
//  H i s t o r y :
//  ________________________________________________________________________
//
//  Date        Who             What
//  xxxx-xx-xx  Spichtinger     xx
/*****************************************************************************/

#include "precomp.h"                                                        // environment specific IncludeFiles
#include "Inc.h"

#ifdef PNDEV_TRACE
    #include "os.tmh"
#endif

#ifdef ALLOC_PRAGMA
    #pragma alloc_text (INIT, DriverEntry)
    #pragma alloc_text (PAGE, fnEvtDriverUnload)
    #pragma alloc_text (PAGE, fnEvtDriverContextCleanup)
    #pragma alloc_text (PAGE, fnEvtDeviceAdd)
//  #pragma alloc_text (PAGE, fnEvtDeviceD0Entry)                           // see function
    #pragma alloc_text (PAGE, fnEvtDeviceD0Exit)
    #pragma alloc_text (PAGE, fnEvtDevicePrepareHw)
    #pragma alloc_text (PAGE, fnEvtDeviceReleaseHw)
//  #pragma alloc_text (PAGE, fnEvtIoctl)
//  #pragma alloc_text (PAGE, fnEvtIsr)
//  #pragma alloc_text (PAGE, fnEvtDpc)
//  #pragma alloc_text (PAGE, fnEvtTimer)
//  #pragma alloc_text (PAGE, fnEvtIntEnable)
//  #pragma alloc_text (PAGE, fnEvtIntDisable)
//  #pragma alloc_text (PAGE, fnThreadSys)
//------------------------------------------------------------------------
//  #pragma alloc_text (PAGE, fnStartUpLockRes)
//  #pragma alloc_text (PAGE, fnShutDownLockRes)
//  #pragma alloc_text (PAGE, fnAcquireSpinLockIrq)
//  #pragma alloc_text (PAGE, fnReleaseSpinLockIrq)
//  #pragma alloc_text (PAGE, fnAcquireSpinLock)
//  #pragma alloc_text (PAGE, fnReleaseSpinLock)
//  #pragma alloc_text (PAGE, fnEnterCritSec)
//  #pragma alloc_text (PAGE, fnLeaveCritSec)
//  #pragma alloc_text (PAGE, fnStartUpThreadResSys)
//  #pragma alloc_text (PAGE, fnShutDownThreadResSys)
//  #pragma alloc_text (PAGE, fnStartUpThreadResDll)
//  #pragma alloc_text (PAGE, fnShutDownThreadResDll)
//  #pragma alloc_text (PAGE, fnSetEventThreadDll)
//  #pragma alloc_text (PAGE, fnGetIoctlBuf)
//  #pragma alloc_text (PAGE, fnIoctlGetDeviceInfoSys)
//  #pragma alloc_text (PAGE, fnIoctlGetDeviceHandleSys)
//  #pragma alloc_text (PAGE, fnIoctlAllocEventId)
//  #pragma alloc_text (PAGE, fnIoctlFreeEventId)
//  #pragma alloc_text (PAGE, fnIoctlDoNetProfi)
//  #pragma alloc_text (PAGE, fnConnectInt)
//  #pragma alloc_text (PAGE, fnDisconnectInt)
//  #pragma alloc_text (PAGE, fnRegisterIsr)
//  #pragma alloc_text (PAGE, fnQueueDpc)
//  #pragma alloc_text (PAGE, fnAllocMemNonPaged)
//  #pragma alloc_text (PAGE, fnFreeMemNonPaged)
//  #pragma alloc_text (PAGE, fnMapMemPhyToVirtual)
//  #pragma alloc_text (PAGE, fnReleaseMemVirtual)
//  #pragma alloc_text (PAGE, fnMapMemVirtualToUser)
//  #pragma alloc_text (PAGE, fnReleaseMemUser)
//  #pragma alloc_text (PAGE, fnDelayThread)
//  #pragma alloc_text (PAGE, fnGetClock)
//  #pragma alloc_text (PAGE, fnComputeRunTimeUsec)
//  #pragma alloc_text (PAGE, fnSetTrace)
//  #pragma alloc_text (PAGE, fnCheckIrql)
//  #pragma alloc_text (PAGE, fnGetResListConfigSpace)
//  #pragma alloc_text (PAGE, fnGetInfoConfigSpace)
//  #pragma alloc_text (PAGE, fnGetBarConfigSpace)
//  #pragma alloc_text (PAGE, fnGetExtendedConfigSpace)
//  #pragma alloc_text (PAGE, fnPrepareConfigSpaceHwReset)
//  #pragma alloc_text (PAGE, fnRestoreConfigSpaceHwReset)
//  #pragma alloc_text (PAGE, fnScanDevice)
//  #pragma alloc_text (PAGE, fnGetBoardType)
//  #pragma alloc_text (PAGE, fnGetFreeIndexEventHandleTable)
//  #pragma alloc_text (PAGE, fnReleaseIndexEventHandleTable)
//  #pragma alloc_text (PAGE, fnMmAllocateContiguousMemory)
//  #pragma alloc_text (PAGE, fnMmFreeContiguousMemory)
#endif

//########################################################################
//  Variables
//########################################################################

uPNDEV_MEM_SYS g_uMemSys;

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
static uPNDEV_VAR_SYS g_uVarSys;

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
static uPNDEV_VAR_SYS g_uVarSys;

#define	PNDEV_ARRAY_DIM	255
static char* 	PciLoc[PNDEV_ARRAY_DIM]	= {0};
static int		lPciLoc					= 0;
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  CBF for DriverEntry
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
// Note:
//  - called directly by the I/O system
//  - if failure: driver will get unloaded immediately
//  - called at PASSIVE_LEVEL

NTSTATUS DriverEntry(   IN PDRIVER_OBJECT  oDriverObjectIn,                 // ptr to the driver object
                        IN PUNICODE_STRING sRegistryPathIn)                 // ptr to a unicode string representing the path to driver-specific key in the registry

{
NTSTATUS                lResult = STATUS_SUCCESS;
WDF_DRIVER_CONFIG       uConfigDriver;
WDF_OBJECT_ATTRIBUTES   uAttributes;
char sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

    #if DBG
        DbgBreakPoint();
        //__asm int 3;
    #endif

    // initialize WPP tracing of WDF
    WPP_INIT_TRACING(   oDriverObjectIn,
                        sRegistryPathIn);

    // set TraceDetail
    RtlStringCchPrintfA(sTraceDetail,
                        _countof(sTraceDetail),
                        "PnDev[x,x,x]: ==== Driver Entry (PnDev_Driver built %s %s)",
                        __DATE__,
                        __TIME__);

    // set trace
    fnSetTrace( ePNDEV_TRACE_LEVEL_INFO,
                ePNDEV_TRACE_CONTEXT_BASIC,
                TRUE,                       // logging
                sTraceDetail);

    // initialize the driver ConfigStructure
    WDF_DRIVER_CONFIG_INIT( &uConfigDriver,
                            fnEvtDeviceAdd);
    uConfigDriver.EvtDriverUnload = fnEvtDriverUnload;

    // register a cleanup callback so that we can call WPP_CLEANUP when the framework driver object is deleted during driver unload
    WDF_OBJECT_ATTRIBUTES_INIT(&uAttributes);
    uAttributes.EvtCleanupCallback = fnEvtDriverContextCleanup;

    // create WDF driver
    lResult = WdfDriverCreate(oDriverObjectIn,
                              sRegistryPathIn,
                              &uAttributes,
                              &uConfigDriver,
                              WDF_NO_HANDLE);

    if  (lResult != STATUS_SUCCESS)
        // error
    {
        // set TraceDetail
        RtlStringCchPrintfA(sTraceDetail,
                            _countof(sTraceDetail),
                            "PnDev[x,x,x]: ########## Error at WdfDriverCreate (status=%!STATUS!)!",
                            lResult);

        // set trace
        fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                    ePNDEV_TRACE_CONTEXT_BASIC,
                    TRUE,                       // logging
                    sTraceDetail);

        // cleanup
        //  - DriverContextCleanup will not be called as we have failed to create WDFDRIVER object itself
        //  - if there would be an error after the WDFDRIVER object is created successfully:
        //      WPP_CLEANUP() need not to be called because DriverContextCleanup will be executed when the framework deletes the DriverObject
        WPP_CLEANUP(oDriverObjectIn);
    }

    // preset global variables
    {
        RtlZeroMemory(  &g_uMemSys,
                        sizeof(uPNDEV_MEM_SYS));
    }

    // allocate contigous shared memory in the host system
    //  - UseCases:
    // (1) NetProfi (CP1616 Board) - This memory will be provided to the first CP1616 if a DeviceOpen is called. This memory is physical contiguous and can be accessed by the board
    //      - Minimum: 0GB ; Maximum - 768 MB (determined by the PCI bridge of the ERTEC400, see Base Address Translation (BAT) registers of the PCI Bridge)
    {
        fnMmAllocateContiguousMemory(   16*1024*1024,               // 16MB
                                        0,
                                        768*1024*1024,              // 0,75GB
                                        &(g_uMemSys.uHostMem[0]));

        fnMmAllocateContiguousMemory(   16*1024*1024,               // 16MB
                                        0,
                                        0xFFFFFFFF,                 // 4,00GB
                                        &(g_uMemSys.uHostMem[1]));

        fnMmAllocateContiguousMemory(   16*1024*1024,               // 16MB
                                        0,
                                        0xFFFFFFFF,                 // 4,00GB
                                        &(g_uMemSys.uHostMem[2]));

        fnMmAllocateContiguousMemory(   16*1024*1024,               // 16MB
                                        0,
                                        0xFFFFFFFF,                 // 4,00GB
                                        &(g_uMemSys.uHostMem[3]));
    }

    return(lResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
// CBF for opening of driver (see adn_drv_register())
//  - ReturnValue is stored at info.user_id

int32_t DriverEntry(adn_drv_info_t* uInfoInOut,
                    int             lFlagIn)
{
int32_t lResult = -1;

    if  (uInfoInOut->state == ADN_DRV_STATE_BUSY)
        // open already done
    {
        // success
        //  - return previous index
        lResult = 0;
    }
    else
    {
    _TCHAR sError[PNDEV_SIZE_STRING_BUF] = {0};

        // preset variables
        {
            RtlZeroMemory(  &g_uVarSys,
                            sizeof(uPNDEV_VAR_SYS));

            // initialize DeviceList
            fnInitList(&g_uVarSys.uListDevice);
        }

        if  (!fnCreateMutex(_countof(sError),
                            &g_uVarSys.uCritSecIoctlDriver,
                            sError))
            // creating DriverMutex ok
        {
            // set trace by returned ErrorString
            fnSetTraceFctError( NULL,
                                sError);
        }
        else
        {
        BOOLEAN bResult1    = FALSE;
        BOOLEAN bResult2    = FALSE;
        BOOLEAN bResult3    = FALSE;
        BOOLEAN bResult4    = FALSE;
        BOOLEAN bResult5    = FALSE;
        BOOLEAN bResult6    = FALSE;
        BOOLEAN bResult7    = FALSE;
        BOOLEAN bResult8    = FALSE;
        BOOLEAN bResult9    = FALSE;
        BOOLEAN bResult10   = FALSE;
        BOOLEAN bResult11   = FALSE;
		BOOLEAN	bResult12	= FALSE;

            // search for CP1616 board
            bResult1 = fnScanDevice(    PNDEV_VENDOR_ID_SIEMENS,
                                        PNDEV_DEVICE_ID_CP1616,
                                        0,
                                        ePNDEV_BOARD_CP1616);

            // search for EB200 board
            bResult2 = fnScanDevice(    PNDEV_VENDOR_ID_SIEMENS,
                                        PNDEV_DEVICE_ID_EB200,
                                        0,
                                        ePNDEV_BOARD_EB200);

            // search for DB-Soc1-PCI board
            bResult3 = fnScanDevice(    PNDEV_VENDOR_ID_SIEMENS,
                                        PNDEV_DEVICE_ID_DB_SOC1_PCI,
                                        0,
                                        ePNDEV_BOARD_DB_SOC1_PCI);

            // search for DB-Soc1-PCIe board
            bResult4 = fnScanDevice(    PNDEV_VENDOR_ID_SIEMENS,
                                        PNDEV_DEVICE_ID_DB_SOC1_PCIE,
                                        0,
                                        ePNDEV_BOARD_DB_SOC1_PCIE);

            // search for FPGA1-Ertec200P board
            bResult5 = fnScanDevice(    PNDEV_VENDOR_ID_SIEMENS,
                                        PNDEV_DEVICE_ID_FPGA1,
                                        PNDEV_SUBSYSTEM_ID_FPGA1_ERTEC200P,
                                        ePNDEV_BOARD_FPGA1_ERTEC200P);

            // search for EB200P board
            bResult6 = fnScanDevice(    PNDEV_VENDOR_ID_SIEMENS,
                                        PNDEV_DEVICE_ID_EB200P,
                                        0,
                                        ePNDEV_BOARD_EB200P);

            // search for I210 board
            bResult7 = fnScanDevice(   PNDEV_VENDOR_ID_INTEL,
                                        PNDEV_DEVICE_ID_I210,
                                        0,
                                        ePNDEV_BOARD_I210);

            // search for DB-EB400-PCIe board
            bResult8 = fnScanDevice(   PNDEV_VENDOR_ID_SIEMENS,
                                        PNDEV_DEVICE_ID_DB_EB400_PCIE,
                                        0,
                                        ePNDEV_BOARD_DB_EB400_PCIE);

			// search for Intel I82574 board
			bResult9 = fnScanDevice(	PNDEV_VENDOR_ID_INTEL,
										PNDEV_DEVICE_ID_I82574,
										0,
										ePNDEV_BOARD_I82574);
			
			// search for Micrel KSZ8841 board
			bResult10 = fnScanDevice(	PNDEV_VENDOR_ID_MICREL,
										PNDEV_DEVICE_ID_MICREL_KSZ8841,
										0,
										ePNDEV_BOARD_MICREL_KSZ8841);
			
			// search for Micrel KSZ8842 board
			bResult11 = fnScanDevice(	PNDEV_VENDOR_ID_MICREL,
										PNDEV_DEVICE_ID_MICREL_KSZ8842,
										0,
										ePNDEV_BOARD_MICREL_KSZ8842);
			
			// search for TI Omap board
			bResult12 = fnScanDevice(	PNDEV_VENDOR_ID_TI,
										PNDEV_DEVICE_ID_TI_OMAP,
										0,
										ePNDEV_BOARD_TI_OMAP);

            if  (   bResult1
                &&  bResult2
                &&  bResult3
                &&  bResult4
                &&  bResult5
                &&  bResult6
                &&  bResult7
                &&  bResult8
                &&  bResult9
                &&  bResult10
                &&  bResult11
				&&	bResult12)
                // success
            {
                // success
                //  - return index for first opening
                lResult = 0;

                // mark DriverState as busy
                uInfoInOut->state = ADN_DRV_STATE_BUSY;

                // allocate ContigousMemory
                {
                    fnMmAllocateContiguousMemory(   16*1024*1024,               // 16MB
                                                    0,
                                                    0,
                                                    &(g_uMemSys.uHostMem[0]));

                    fnMmAllocateContiguousMemory(   16*1024*1024,               // 16MB
                                                    0,
                                                    0,
                                                    &(g_uMemSys.uHostMem[1]));

                    fnMmAllocateContiguousMemory(   16*1024*1024,               // 16MB
                                                    0,
                                                    0,
                                                    &(g_uMemSys.uHostMem[2]));

                    fnMmAllocateContiguousMemory(   16*1024*1024,               // 16MB
                                                    0,
                                                    0,
                                                    &(g_uMemSys.uHostMem[3]));
                }
            }
        }
    }

    return(lResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------

int DriverEntry(void)
{
int     lResult     = LINUX_ERROR;
int     lResultTmp  = LINUX_ERROR;
_TCHAR  sError[PNDEV_SIZE_STRING_BUF] = {0};

    // preset variables
    {
        RtlZeroMemory(  &g_uVarSys,
                        sizeof(uPNDEV_VAR_SYS));

        // initialize DeviceList
        fnInitList(&g_uVarSys.uListDevice);
    }

    // set function operaterations
    g_uVarSys.uFctOps.owner             = THIS_MODULE;
    g_uVarSys.uFctOps.unlocked_ioctl    = fnEvtIoctl;
    g_uVarSys.uFctOps.compat_ioctl		= fnEvtIoctl;
    g_uVarSys.uFctOps.mmap              = fnEvtMmap;
    g_uVarSys.uFctOps.open              = fnOpenDriver;
    g_uVarSys.uFctOps.release           = fnCloseDriver;

    // get MajorNumber and MinorNumber in uDev
    lResultTmp = alloc_chrdev_region(   &g_uVarSys.uDev,
                                        0,
                                        1,
                                        PNDEV_DRV_NAME);

    if  (lResultTmp != LINUX_OK)
        // error
    {
        // set ErrorString
        _tcscpy_s(  sError, _countof(sError), _TEXT(__FUNCTION__));
        _tcsncat_s( sError, _countof(sError), _TEXT("(): Error at register_chrdev()!"), _TRUNCATE);

        // set trace by returned ErrorString
        fnSetTraceFctError( NULL,
                            sError);
    }
    else
    {
    //struct class* pDrvClass = g_uVarSys.pDrvClass;

        // create class
        g_uVarSys.pDrvClass = class_create(	THIS_MODULE,
                                    		PNDEV_DRV_NAME);

        if (g_uVarSys.pDrvClass == NULL)
        {
            // unregister region
            unregister_chrdev_region(   g_uVarSys.uDev,
                                        1);

            // set ErrorString
            _tcscpy_s(  sError, _countof(sError), _TEXT(__FUNCTION__));
            _tcsncat_s( sError, _countof(sError), _TEXT("(): Error at class_create()!"), _TRUNCATE);

            // set trace by returned ErrorString
            fnSetTraceFctError( NULL,
                                sError);
        }
        else
        {
        struct device* pDrvDevice = &g_uVarSys.uDrvDevice;

            // create device
            pDrvDevice = device_create( g_uVarSys.pDrvClass,
                                        NULL,
                                        g_uVarSys.uDev,
                                        NULL,
                                        PNDEV_DRV_NAME);

            if  (pDrvDevice == NULL)
                // error
            {
                // destroy class
                class_destroy(g_uVarSys.pDrvClass);

                // unregister region
                unregister_chrdev_region(   g_uVarSys.uDev,
                                            1);

                // set ErrorString
                _tcscpy_s(  sError, _countof(sError), _TEXT(__FUNCTION__));
                _tcsncat_s( sError, _countof(sError), _TEXT("(): Error at device_create()!"), _TRUNCATE);

                // set trace by returned ErrorString
                fnSetTraceFctError( NULL,
                                    sError);
            }
            else
            {
            struct cdev* pDrvCdev = &g_uVarSys.uDrvCdev;

                // init Device with corresponding FunctionOperations
                cdev_init(pDrvCdev, &g_uVarSys.uFctOps);

                // add Device
                lResultTmp = cdev_add(  pDrvCdev,
                                        g_uVarSys.uDev,
                                        1);

                if  (lResultTmp != LINUX_OK)
                    // error
                {
                    // destroy device
                    device_destroy( g_uVarSys.pDrvClass,
                                    g_uVarSys.uDev);

                    // destroy class
                    class_destroy(g_uVarSys.pDrvClass);

                    // unregister region
                    unregister_chrdev_region(   g_uVarSys.uDev,
                                                1);

                    // set ErrorString
                    _tcscpy_s(  sError, _countof(sError), _TEXT(__FUNCTION__));
                    _tcsncat_s( sError, _countof(sError), _TEXT("(): Error at device_create()!"), _TRUNCATE);

                    // set trace by returned ErrorString
                    fnSetTraceFctError( NULL,
                                        sError);
                }
                else
                {
                    if  (!fnCreateMutex(_countof(sError),
                                        &g_uVarSys.uCritSecIoctlDriver,
                                        sError))
                        // creating DriverMutex ok
                    {
                        // set trace by returned ErrorString
                        fnSetTraceFctError( NULL,
                                            sError);
                    }
                    else
                    {
                    BOOLEAN bResult1    = FALSE;
                    BOOLEAN bResult2    = FALSE;
                    BOOLEAN bResult3    = FALSE;
                    BOOLEAN bResult4    = FALSE;
                    BOOLEAN bResult5    = FALSE;
                    BOOLEAN bResult6    = FALSE;
                    BOOLEAN bResult7    = FALSE;
                    BOOLEAN bResult8    = FALSE;
                    BOOLEAN bResult9    = FALSE;
                    BOOLEAN bResult10   = FALSE;
                    BOOLEAN bResult11   = FALSE;
					BOOLEAN bResult12   = FALSE;

                        // search for CP1616 board
                        bResult1 = fnScanDevice(    PNDEV_VENDOR_ID_SIEMENS,
                                                    PNDEV_DEVICE_ID_CP1616,
                                                    0,
                                                    ePNDEV_BOARD_CP1616);

                        // search for EB200 board
                        bResult2 = fnScanDevice(    PNDEV_VENDOR_ID_SIEMENS,
                                                    PNDEV_DEVICE_ID_EB200,
                                                    0,
                                                    ePNDEV_BOARD_EB200);

                        // search for DB-Soc1-PCI board
                        bResult3 = fnScanDevice(    PNDEV_VENDOR_ID_SIEMENS,
                                                    PNDEV_DEVICE_ID_DB_SOC1_PCI,
                                                    0,
                                                    ePNDEV_BOARD_DB_SOC1_PCI);

                        // search for DB-Soc1-PCIe board
                        bResult4 = fnScanDevice(    PNDEV_VENDOR_ID_SIEMENS,
                                                    PNDEV_DEVICE_ID_DB_SOC1_PCIE,
                                                    0,
                                                    ePNDEV_BOARD_DB_SOC1_PCIE);

                        // search for FPGA1-Ertec200P board
                        bResult5 = fnScanDevice(    PNDEV_VENDOR_ID_SIEMENS,
                                                    PNDEV_DEVICE_ID_FPGA1,
                                                    PNDEV_SUBSYSTEM_ID_FPGA1_ERTEC200P,
                                                    ePNDEV_BOARD_FPGA1_ERTEC200P);

                        // search for EB200P board
                        bResult6 = fnScanDevice(    PNDEV_VENDOR_ID_SIEMENS,
                                                    PNDEV_DEVICE_ID_EB200P,
                                                    0,
                                                    ePNDEV_BOARD_EB200P);

                        // search for I210 board
                        bResult7 = fnScanDevice(   PNDEV_VENDOR_ID_INTEL,
                                                    PNDEV_DEVICE_ID_I210,
                                                    0,
                                                    ePNDEV_BOARD_I210);

                        // search for DB-EB400-PCIe board
                        bResult8 = fnScanDevice(   PNDEV_VENDOR_ID_SIEMENS,
                                                    PNDEV_DEVICE_ID_DB_EB400_PCIE,
                                                    0,
                                                    ePNDEV_BOARD_DB_EB400_PCIE);

						// search for Intel I82574 board
						bResult9 = fnScanDevice(	PNDEV_VENDOR_ID_INTEL,
													PNDEV_DEVICE_ID_I82574,
													0,
													ePNDEV_BOARD_I82574);
						
						// search for Micrel KSZ8841 board
						bResult10 = fnScanDevice(	PNDEV_VENDOR_ID_MICREL,
													PNDEV_DEVICE_ID_MICREL_KSZ8841,
													0,
													ePNDEV_BOARD_MICREL_KSZ8841);
						
						// search for Micrel KSZ8842 board
						bResult11 = fnScanDevice(	PNDEV_VENDOR_ID_MICREL,
													PNDEV_DEVICE_ID_MICREL_KSZ8842,
													0,
													ePNDEV_BOARD_MICREL_KSZ8842);
						
						// search for TI Omap board
						bResult12 = fnScanDevice(	PNDEV_VENDOR_ID_TI,
													PNDEV_DEVICE_ID_TI_OMAP,
													0,
													ePNDEV_BOARD_TI_OMAP);

                        if  (   bResult1
                            &&  bResult2
                            &&  bResult3
                            &&  bResult4
                            &&  bResult5
                            &&  bResult6
                            &&  bResult7
                            &&  bResult8
                            &&  bResult9
                            &&  bResult10
                            &&  bResult11
                            &&	bResult12)
                            // success
                        {
                            // success
                            //  - return index for first opening
                                lResult = LINUX_OK;

                            // allocate ContigousMemory
                            {
                                fnMmAllocateContiguousMemory(   16*1024*1024,               // 16MB
                                                                0,
                                                                768*1024*1024,
                                                                &(g_uMemSys.uHostMem[0]));   // get memory below 768MB

                                fnMmAllocateContiguousMemory(   16*1024*1024,               // 16MB
                                                                0,
                                                                0xFFFFFFFF,                 // get memoryblocks with address within 32bit
                                                                &(g_uMemSys.uHostMem[1]));

                                fnMmAllocateContiguousMemory(   16*1024*1024,               // 16MB
                                                                0,
                                                                0xFFFFFFFF,                 // get memoryblocks with address within 32bit
                                                                &(g_uMemSys.uHostMem[2]));

                                fnMmAllocateContiguousMemory(   16*1024*1024,               // 16MB
                                                                0,
                                                                0xFFFFFFFF,                 // get memoryblocks with address within 32bit
                                                                &(g_uMemSys.uHostMem[3]));
                            }
                        }
                        else
                        {
                            // set ErrorString
                            _tcscpy_s(  sError, _countof(sError), _TEXT(__FUNCTION__));
                            _tcsncat_s( sError, _countof(sError), _TEXT("(): Error at fnScanDevice()!"), _TRUNCATE);

                            // set trace by returned ErrorString
                            fnSetTraceFctError( NULL,
                                                sError);

                        }
                    }
                }
            }
        }
    }

    return(lResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  CBF for DriverUnload
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
// Note:
//  - called at PASSIVE_LEVEL

void fnEvtDriverUnload(IN WDFDRIVER hDriverIn)
{
char sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

    UNREFERENCED_PARAMETER(hDriverIn);

    PAGED_CODE();

    // set TraceDetail
    RtlStringCchPrintfA(sTraceDetail,
                        _countof(sTraceDetail),
                        "PnDev[x,x,x]: ==== Driver Unload");


    // free HostMem
    fnMmFreeContiguousMemory(&(g_uMemSys.uHostMem[0]));
    fnMmFreeContiguousMemory(&(g_uMemSys.uHostMem[1]));
    fnMmFreeContiguousMemory(&(g_uMemSys.uHostMem[2]));
    fnMmFreeContiguousMemory(&(g_uMemSys.uHostMem[3]));

    // set trace
    fnSetTrace( ePNDEV_TRACE_LEVEL_INFO,
                ePNDEV_TRACE_CONTEXT_BASIC,
                TRUE,                       // logging
                sTraceDetail);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
// CBF for closing of driver (see adn_drv_register())

int32_t fnEvtDriverUnload(adn_drv_info_t* uInfoIn)
{
int32_t lResult = ADN_ERROR;
_TCHAR  sError[PNDEV_SIZE_STRING_BUF] = {0};

    // preset
    lResult = ADN_OK;

    // release all devices
    {
        if  (g_uVarSys.lCtrDevice != 0)
            // device exist
        {
        uLIST_HEADER*       pList   = NULL;
        DEVICE_EXTENSION*   pDevExt = NULL;

            pList = &g_uVarSys.uListDevice;

            while   (!fnListEmpty(pList))
                    // DeviceList not empty
            {
                // get first block of DeviceList
                pDevExt = (DEVICE_EXTENSION*) pList->pNext;

                // call CBF for releasing hardware
                fnEvtDeviceReleaseHw(pDevExt);

                // remove DeviceExtension from DeviceList
                fnRemoveBlkFromList(&g_uVarSys.uListDevice,
                                    ((uLIST_HEADER*) pDevExt));

                g_uVarSys.lCtrDevice--;

                // free DeviceExtension
                fnFreeMemNonPaged(  pDevExt,
                                    TAG('I','n','i','t'));
            }
        }
    }

    // free HostMem
    fnMmFreeContiguousMemory(&(g_uMemSys.uHostMem[0]));
    fnMmFreeContiguousMemory(&(g_uMemSys.uHostMem[1]));
    fnMmFreeContiguousMemory(&(g_uMemSys.uHostMem[2]));
    fnMmFreeContiguousMemory(&(g_uMemSys.uHostMem[3]));

    if  (!fnDeleteMutex(&g_uVarSys.uCritSecIoctlDriver,
                        _countof(sError),
                        sError))
        // error at deleting DriverMutex
    {
        // set trace by returned ErrorString
        fnSetTraceFctError( NULL,
                            sError);
    }

    // preset variables
    {
        RtlZeroMemory(  &g_uVarSys,
                        sizeof(uPNDEV_VAR_SYS));

        // initialize DeviceList
        fnInitList(&g_uVarSys.uListDevice);
    }

    // mark interface as free
    uInfoIn->state = ADN_DRV_STATE_FREE;

    return(lResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------

void fnEvtDriverUnload(void)
{
_TCHAR  sError[PNDEV_SIZE_STRING_BUF] = {0};

    // release all devices
    {
        if  (g_uVarSys.lCtrDevice != 0)
            // device exist
        {
        uLIST_HEADER*       pList   = NULL;
        DEVICE_EXTENSION*   pDevExt = NULL;

            pList = &g_uVarSys.uListDevice;

            while   (!fnListEmpty(pList))
                    // DeviceList not empty
            {
                // get first block of DeviceList
                pDevExt = (DEVICE_EXTENSION*) pList->pNext;

                // call CBF for releasing hardware
                fnEvtDeviceReleaseHw(pDevExt);

                // remove DeviceExtension from DeviceList
                fnRemoveBlkFromList(&g_uVarSys.uListDevice,
                                    ((uLIST_HEADER*) pDevExt));

                g_uVarSys.lCtrDevice--;

                // free DeviceExtension
                fnFreeMemNonPaged(  pDevExt,
                                    TAG('I','n','i','t'));
            }
        }
    }

    // free HostMem
    fnMmFreeContiguousMemory(&(g_uMemSys.uHostMem[0]));
    fnMmFreeContiguousMemory(&(g_uMemSys.uHostMem[1]));
    fnMmFreeContiguousMemory(&(g_uMemSys.uHostMem[2]));
    fnMmFreeContiguousMemory(&(g_uMemSys.uHostMem[3]));

    if  (!fnDeleteMutex(&g_uVarSys.uCritSecIoctlDriver,
                        _countof(sError),
                        sError))
        // error at deleting DriverMutex
    {
        // set trace by returned ErrorString
        fnSetTraceFctError( NULL,
                            sError);
    }

    // delete driver
    {
        // delete device
        cdev_del(&g_uVarSys.uDrvCdev);

        // destroy device
        device_destroy( g_uVarSys.pDrvClass,
                        g_uVarSys.uDev);

        // destroy class
        class_destroy(  g_uVarSys.pDrvClass);

        // unregister region
        unregister_chrdev_region(   g_uVarSys.uDev,
                                    1);
    }

    // preset variables
    {
        RtlZeroMemory(  &g_uVarSys,
                        sizeof(uPNDEV_VAR_SYS));

        // initialize DeviceList
        fnInitList(&g_uVarSys.uListDevice);
    }

}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  CBF for DriverContextCleanup
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
// Note:
//  - free all resources allocated in DriverEntry
//  - called at PASSIVE_LEVEL

void fnEvtDriverContextCleanup(IN WDFDRIVER hDriverIn)
{
    PAGED_CODE ();

    // cleanup
    WPP_CLEANUP(WdfDriverWdmGetDriverObject(hDriverIn));
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  CBF for AddDevice
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
// Note:
//  - called by the framework in response to AddDevice call from the PnP manager
//  - called at PASSIVE_LEVEL

NTSTATUS fnEvtDeviceAdd(IN WDFDRIVER        hDriverIn,
                        IN PWDFDEVICE_INIT  hDeviceInitIn)
{
NTSTATUS                        lResult = STATUS_UNSUCCESSFUL;
ePNDEV_BOARD                    eBoard  = ePNDEV_BOARD_INVALID;
ULONG                           lSize   = 0;
WDF_PNPPOWER_EVENT_CALLBACKS    uPnpPowerCallbacks;
WDF_OBJECT_ATTRIBUTES           uAttributes;
WDFDEVICE                       hDevice;
WDF_IO_QUEUE_CONFIG             uConfigIoQueue;
WDFQUEUE                        hQueue;
_TCHAR                          sHardwareId[PNDEV_SIZE_STRING_BUF]      = {0};
char                            sTraceDetail[PNDEV_SIZE_STRING_TRACE]   = {0};

    // Note:
    //  - Driver should register all the PNP, power and Io callbacks
    //  - Driver should register interfaces and allocate other software resources required by the device
    //  - Driver can query any interfaces or get the config space information from the bus driver
    //      but cannot access hardware registers or initialize the device

    UNREFERENCED_PARAMETER(hDriverIn);

    PAGED_CODE();

    // set TraceDetail
    RtlStringCchPrintfA(sTraceDetail,
                        _countof(sTraceDetail),
                        "PnDev[x,x,x]: ---- %s",
                        __FUNCTION__);

    // set trace
    fnSetTrace( ePNDEV_TRACE_LEVEL_INFO,
                ePNDEV_TRACE_CONTEXT_BASIC,
                FALSE,                      // logging
                sTraceDetail);

    // get device property "HardwareId"
    //  - Note: must be done before WdfDeviceCreate, otherwise hDeviceInitIn=NULL
    lResult = WdfFdoInitQueryProperty(  hDeviceInitIn,
                                        DevicePropertyHardwareID,
                                        sizeof(sHardwareId),
                                        sHardwareId,
                                        &lSize);
    if  (lResult != STATUS_SUCCESS)
        // error
    {
        // set TraceDetail
        RtlStringCchPrintfA(sTraceDetail,
                            _countof(sTraceDetail),
                            "PnDev[x,x,x]: ########## Error at WdfFdoInitQueryProperty() (status=%!STATUS!)!",
                            lResult);

        // set trace
        fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                    ePNDEV_TRACE_CONTEXT_BASIC,
                    TRUE,                       // logging
                    sTraceDetail);
    }
    else
    {
    char sHardwareIdAscii[PNDEV_SIZE_STRING_BUF_SHORT] = {0};

        // convert _TCHAR to ASCII string
        fnConvertStringTcharToChar( sHardwareId,
                                    sHardwareIdAscii);

        // set TraceDetail
        RtlStringCchPrintfA(sTraceDetail,
                            _countof(sTraceDetail),
                            "PnDev[x,x,x]: ++++ HardwareId=%s",
                            sHardwareIdAscii);

        // set trace
        fnSetTrace( ePNDEV_TRACE_LEVEL_INFO,
                    ePNDEV_TRACE_CONTEXT_BASIC,
                    TRUE,                       // logging
                    sTraceDetail);

        // preset
        lResult = STATUS_UNSUCCESSFUL;

        // get BoardType
        eBoard = fnGetBoardType(sHardwareId);

        if  (eBoard != ePNDEV_BOARD_INVALID)
            // success
        {
            WdfDeviceInitSetIoType( hDeviceInitIn,
                                    WdfDeviceIoBuffered);

            // zero out the PnpPowerCallbacks structure
            WDF_PNPPOWER_EVENT_CALLBACKS_INIT(&uPnpPowerCallbacks);

            // set callbacks for any of the functions we are interested in
            //  - if no callback is set, framework will take the default action by itself
            uPnpPowerCallbacks.EvtDevicePrepareHardware = fnEvtDevicePrepareHw;
            uPnpPowerCallbacks.EvtDeviceReleaseHardware = fnEvtDeviceReleaseHw;

            // these two callbacks set up and tear down hardware state that must be done every time the device moves in and out of the D0-working state
            uPnpPowerCallbacks.EvtDeviceD0Entry = fnEvtDeviceD0Entry;
            uPnpPowerCallbacks.EvtDeviceD0Exit  = fnEvtDeviceD0Exit;

            // register the PnP callbacks
            WdfDeviceInitSetPnpPowerEventCallbacks( hDeviceInitIn,
                                                    &uPnpPowerCallbacks);

            // initialize FDO attributes
            WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&uAttributes,
                                                    DEVICE_EXTENSION);

            // opting for SynchronizationScopeDevice
            //  - we tell the framework to synchronize callbacks events of all the objects directly associated with the device
            //  - in this driver we will associate queues and DpcForIsr
            //  - by doing that we don't have to worry about synchronizing access to device-context by IoEvents and DpcForIsr because they would not be concurrently ever.
            //      Framework will serialize them by using an internal device-lock
            //      -> all queue callbacks, cancel routine and DpcForIsr are serialized at the device level
            //      -> we don't have worry about any synchronization issues.
            // klsp:
            //  - cannot be used because IOCTLs would then be called at DISPATCH_LEVEL
            //  - many system functions need PASSIVE_LEVEL!
            //uAttributes.SynchronizationScope = WdfSynchronizationScopeDevice;

            // create the device
            lResult = WdfDeviceCreate(  &hDeviceInitIn,
                                        &uAttributes,
                                        &hDevice);

            if  (lResult != STATUS_SUCCESS)
                // error
            {
                // set TraceDetail
                RtlStringCchPrintfA(sTraceDetail,
                                    _countof(sTraceDetail),
                                    "PnDev[x,x,x]: ########## Error at WdfDeviceCreate() (status=%!STATUS!)!",
                                    lResult);

                // set trace
                fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                            ePNDEV_TRACE_CONTEXT_BASIC,
                            TRUE,                       // logging
                            sTraceDetail);
            }
            else
            {
                // preset
                lResult = STATUS_UNSUCCESSFUL;

                // set TraceDetail
                RtlStringCchPrintfA(sTraceDetail,
                                    _countof(sTraceDetail),
                                    "PnDev[x,x,x]: AddDevice [PDO=0xllx, FDO=0xllx]",
                                    WdfDeviceWdmGetPhysicalDevice(hDevice),
                                    WdfDeviceWdmGetDeviceObject(hDevice));

                // set trace
                fnSetTrace( ePNDEV_TRACE_LEVEL_INFO,
                            ePNDEV_TRACE_CONTEXT_BASIC,
                            FALSE,                      // logging
                            sTraceDetail);

                // register I/O callbacks:
                //  - create a sequential queue so that requests are dispatched to the driver one at a time.
                //      Until the driver completes a dispatched request another request will be not presented to the driver.
                //  - a default queue gets all the requests that are not configure-forwarded using WdfDeviceConfigureRequestDispatching.
                // klsp:
                //  - cannot be used because ObReferenceObjectByHandle() must be called in context of the UserThread
                //      -> WdfIoQueueDispatchParallel, synchronization of IOCTLs by CriticalSection
                WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE( &uConfigIoQueue,
                                                        WdfIoQueueDispatchParallel);
                                                        //WdfIoQueueDispatchSequential);    //klsp

                uConfigIoQueue.EvtIoDeviceControl = fnEvtIoctl;

                WDF_OBJECT_ATTRIBUTES_INIT(&uAttributes);
                uAttributes.ExecutionLevel = WdfExecutionLevelPassive;      //klsp

                lResult = WdfIoQueueCreate( hDevice,
                                            &uConfigIoQueue,
                                            //WDF_NO_OBJECT_ATTRIBUTES,     //klsp
                                            &uAttributes,
                                            &hQueue );

                if  (lResult != STATUS_SUCCESS)
                    // error
                {
                    // set TraceDetail
                    RtlStringCchPrintfA(sTraceDetail,
                                        _countof(sTraceDetail),
                                        "PnDev[x,x,x]: ########## Error at WdfIoQueueCreate() (status=%!STATUS!)!",
                                        lResult);

                    // set trace
                    fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                                ePNDEV_TRACE_CONTEXT_BASIC,
                                TRUE,                       // logging
                                sTraceDetail);
                }
                else
                {
                    // preset
                    lResult = STATUS_UNSUCCESSFUL;

                    // tell the framework that this device will need an interface
                    lResult = WdfDeviceCreateDeviceInterface(   hDevice,
                                                                (LPGUID) &GUID_PNDEV_IF,
                                                                NULL);

                    if  (lResult != STATUS_SUCCESS)
                        // error
                    {
                        // set TraceDetail
                        RtlStringCchPrintfA(sTraceDetail,
                                            _countof(sTraceDetail),
                                            "PnDev[x,x,x]: ########## Error at WdfDeviceCreateDeviceInterface() (status=%!STATUS!)!",
                                            lResult);

                        // set trace
                        fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                                    ePNDEV_TRACE_CONTEXT_BASIC,
                                    TRUE,                       // logging
                                    sTraceDetail);
                    }
                    else
                    {
                    ULONG           lSize = 0;
                    uPNDEV_PCI_LOC  uPciLoc;

                        // preset
                        {
                            lResult = STATUS_UNSUCCESSFUL;

                            RtlZeroMemory(  &uPciLoc,
                                            sizeof(uPNDEV_PCI_LOC));
                        }

                        // get device property "location"
                        lResult = WdfDeviceQueryProperty(   hDevice,
                                                            DevicePropertyLocationInformation,
                                                            sizeof(uPciLoc.sString),
                                                            uPciLoc.sString,
                                                            &lSize);

                        if  (lResult != STATUS_SUCCESS)
                            // error
                        {
                            // set TraceDetail
                            RtlStringCchPrintfA(sTraceDetail,
                                                _countof(sTraceDetail),
                                                "PnDev[x,x,x]: ########## Error at WdfDeviceQueryProperty() (status=%!STATUS!)!",
                                                lResult);

                            // set trace
                            fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                                        ePNDEV_TRACE_CONTEXT_BASIC,
                                        TRUE,                       // logging
                                        sTraceDetail);
                        }
                        else
                        {
                        DEVICE_EXTENSION*   pDevExt = NULL;
                        _TCHAR              sError[PNDEV_SIZE_STRING_BUF] = {0};

                            // get the DeviceExtension and initialize it
                            //  - fnGetDeviceContext is an inline function defined by WDF_DECLARE_CONTEXT_TYPE_WITH_NAME macro in the private header file.
                            //      This function will do the type checking and return the device context.
                            //      If you pass a wrong object handle it will return NULL and assert if run under framework verifier mode.
                            pDevExt = fnGetDeviceContext(hDevice);

                            if  (!fnAddDevice(  pDevExt,
                                                hDevice,
                                                TRUE,           // Windows supports DPC
                                                eBoard,
                                                &uPciLoc,
                                                _countof(sError),
                                                sError))
                                // error at AddDevice
                            {
                                // set trace by returned ErrorString
                                fnSetTraceFctError( pDevExt,
                                                    sError);
                            }
                            else
                            {
                            	
                                // Note:
                                //  - now PciLocShort is valid

                                if  (!fnConnectInt( pDevExt,
                                                    _countof(sError),
                                                    sError))
                                    // error at connecting interrupt
                                    //  - Windows: WdfInterruptCreate() must be called at AddDevice, before WdfDevice is started (see docu, otherwise STATUS_INVALID_DEVICE_STATE)
                                {
                                    // set trace by returned ErrorString
                                    fnSetTraceFctError( pDevExt,
                                                        sError);
                                }
                                else
                                {
                                    // LegIntConnected
                                    pDevExt->bLegIntConnected = TRUE;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return(lResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
BOOLEAN fnEvtDeviceAdd( DEVICE_EXTENSION*   pDevExtIn,
                        ePNDEV_BOARD        eBoardIn,
                        UINT32              lNoBusIn,
                        UINT32              lNoDeviceIn,
                        UINT32              lNoFunctionIn)
{
BOOLEAN         bResult = FALSE;
uPNDEV_PCI_LOC  uPciLoc;
_TCHAR          sError[PNDEV_SIZE_STRING_BUF] = {0};

    // preset
    RtlZeroMemory(  &uPciLoc,
                    sizeof(uPNDEV_PCI_LOC));

    // set PciLoc
    uPciLoc.uNo.lBus        = lNoBusIn;
    uPciLoc.uNo.lDevice     = lNoDeviceIn;
    uPciLoc.uNo.lFunction   = lNoFunctionIn;

    if  (!fnAddDevice(  pDevExtIn,
                        NULL,
                        FALSE,          // ADONIS does not support DPC
                        eBoardIn,
                        &uPciLoc,
                        _countof(sError),
                        sError))
        // error at AddDevice
    {
        // set trace by returned ErrorString
        fnSetTraceFctError( pDevExtIn,
                            sError);
    }
    else
    {
        // success
        bResult = TRUE;

        // Note:
        //  - now PciLocShort is valid and can be used for RtlStringCchPrintfA() / fnSetTrace()
    }

    return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN fnEvtDeviceAdd( DEVICE_EXTENSION*   pDevExtIn,
                        ePNDEV_BOARD        eBoardIn,
                        UINT32              lNoBusIn,
                        UINT32              lNoDeviceIn,
                        UINT32              lNoFunctionIn)
{
BOOLEAN         bResult = FALSE;

uPNDEV_PCI_LOC  uPciLoc;
_TCHAR          sError[PNDEV_SIZE_STRING_BUF] = {0};

    // preset
    RtlZeroMemory(  &uPciLoc,
                    sizeof(uPNDEV_PCI_LOC));

    // set PciLoc
    uPciLoc.uNo.lBus        = lNoBusIn;
    uPciLoc.uNo.lDevice     = lNoDeviceIn;
    uPciLoc.uNo.lFunction   = lNoFunctionIn;

    if  (!fnAddDevice(  pDevExtIn,
                        NULL,
                        FALSE,          // LINUX does not support DPC
                        eBoardIn,
                        &uPciLoc,
                        _countof(sError),
                        sError))
        // error at AddDevice
    {
        // set trace by returned ErrorString
        fnSetTraceFctError( pDevExtIn,
                            sError);
    }
    else
    {
        // success
        bResult = TRUE;

        // Note:
        //  - now PciLocShort is valid and can be used for RtlStringCchPrintfA() / fnSetTrace()
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
//  CBF for start of device D0 state
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
// Note:
//  - called whenever the device enters the D0 state (happens when the device is started, restarted and when it has been powered off)
//  - if failure: device stack will be torn down
//  - called at PASSIVE_LEVEL

NTSTATUS fnEvtDeviceD0Entry(IN WDFDEVICE                hDeviceIn,
                            IN WDF_POWER_DEVICE_STATE   ePreviousStateIn)   // state the device was in before this callback was invoked
{
DEVICE_EXTENSION*   pDevExt;
NTSTATUS            lResult = STATUS_UNSUCCESSFUL;
char                sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

    // Note:
    //  - this routine prepares the device for use
    //  - interrupts will not be enabled at the time that this routine is called. They will be enabled after this callback completes.
    //  - this function is not marked pageable because this function is in the device power up path.
    //      When a function is marked pagable and the code section is paged out, it will generate a page fault which could impact the
    //      fast resume behavior because the client driver will have to wait until the system drivers can service this page fault

    UNREFERENCED_PARAMETER(ePreviousStateIn);

    pDevExt = fnGetDeviceContext(hDeviceIn);

    // set TraceDetail
    RtlStringCchPrintfA(sTraceDetail,
                        _countof(sTraceDetail),
                        "PnDev[%s]: ---- %s",
                        pDevExt->sPciLocShortAscii,
                        __FUNCTION__);

    // set trace
    fnSetTrace( ePNDEV_TRACE_LEVEL_INFO,
                ePNDEV_TRACE_CONTEXT_BASIC,
                FALSE,                      // logging
                sTraceDetail);

    lResult = STATUS_SUCCESS;

    return(lResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  CBF for end of device D0 state
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
// Note:
//  - called whenever the device leaves the D0 state (happens when the device is stopped, removed and when it is powered off)
//  - if failure: device stack will be torn down
//  - called at PASSIVE_LEVEL

NTSTATUS fnEvtDeviceD0Exit( IN WDFDEVICE                hDeviceIn,
                            IN WDF_POWER_DEVICE_STATE   eTargetStateIn)     // state the device will go to when this callback completes
{
NTSTATUS            lResult = STATUS_UNSUCCESSFUL;
DEVICE_EXTENSION*   pDevExt;
char                sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

    // Note:
    //  - this routine undoes anything done in fnEvtDeviceD0Entry
    //  - the device is still in D0 when this callback is invoked, which means that the driver can still touch hardware in this routine
    //  - interrupts have already been disabled by the time that this callback is invoked

    PAGED_CODE();

    pDevExt = fnGetDeviceContext(hDeviceIn);

    // set TraceDetail
    RtlStringCchPrintfA(sTraceDetail,
                        _countof(sTraceDetail),
                        "PnDev[%s]: ---- %s",
                        pDevExt->sPciLocShortAscii,
                        __FUNCTION__);

    // set trace
    fnSetTrace( ePNDEV_TRACE_LEVEL_INFO,
                ePNDEV_TRACE_CONTEXT_BASIC,
                FALSE,                      // logging
                sTraceDetail);

    switch (eTargetStateIn)
    {
        case WdfPowerDeviceD1:
        case WdfPowerDeviceD2:
        case WdfPowerDeviceD3:
        {
            // Note:
            //  - fill in any code to save hardware state here
            //  - fill in any code to put the device in a low-power state here

            break;
        }
        case WdfPowerDevicePrepareForHibernation:
        {
            // Note:
            //  - fill in any code to save hardware state here
            //  - do not put in any code to shut the device off
            //  - if this device cannot support being in the paging path (or being a parent or grandparent of a paging path device) then this whole case can be deleted

            break;
        }
        case WdfPowerDeviceD3Final:
        default:
        {
            // reset the hardware, as we are shutting down for the last time
            fnShutdown(pDevExt);

            break;
        }
    }

    lResult = STATUS_SUCCESS;

    return(lResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  CBF for preparing hardware
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
// Note:
//  - only be called as a device starts or restarts, not every time the device moves into the D0 state
//  - consequently most hardware initialization belongs elsewhere
//  - if failure: device stack will be torn down
//  - called at PASSIVE_LEVEL

NTSTATUS fnEvtDevicePrepareHw(  WDFDEVICE       hDeviceIn,
                                WDFCMRESLIST    hResIn,                     // raw PnP resources associated with the device. Most of the time, these aren't useful for a PCI device
                                WDFCMRESLIST    hResTranslatedIn)           // translated PnP resources associated with the device. This is what is important to a PCI device
{
NTSTATUS            lResult         = STATUS_DEVICE_CONFIGURATION_ERROR;
DEVICE_EXTENSION*   pDevExt         = NULL;
_TCHAR              sError[PNDEV_SIZE_STRING_BUF] = {0};

    // Note:
    //  - performs whatever initialization is needed to setup the device, setting up a DMA channel or mapping any I/O port resources

    UNREFERENCED_PARAMETER(hResIn);

    PAGED_CODE();

    pDevExt = fnGetDeviceContext(hDeviceIn);

    // Note:
    //  - this function is always called if driver is installed or activated
    //      -> PowerOnState is not ensured!
    //      -> there may be problems at InitHw
    //  - execute always HwReset

    if  (!fnPrepareHw(  pDevExt,
                        hResTranslatedIn,
                        ePNDEV_RESET_ACTION_REINIT_HW,
                        _countof(sError),
                        sError))
        // error at preparing hardware
    {
        // set trace by returned ErrorString
        fnSetTraceFctError( pDevExt,
                            sError);
    }
    else
    {
        // success
        lResult = STATUS_SUCCESS;
    }

    return(lResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
BOOLEAN fnEvtDevicePrepareHw(DEVICE_EXTENSION* pDevExtIn)
{
BOOLEAN             bResult         = FALSE;
ePNDEV_RESET_ACTION eResetAction    = ePNDEV_RESET_ACTION_INVALID;
_TCHAR              sError[PNDEV_SIZE_STRING_BUF] = {0};

    if  (!fnPrepareHw(  pDevExtIn,
                        NULL,
                        ePNDEV_RESET_ACTION_REINIT_HW,
                        _countof(sError),
                        sError))
        // error at preparing hardware
    {
        // set trace by returned ErrorString
        fnSetTraceFctError( pDevExtIn,
                            sError);
    }
    else
    {
        #if (PNDEV_CONNECT_LEG_INT == YES)

            if  (!fnConnectInt( pDevExtIn,
                                _countof(sError),
                                sError))
                // error at connecting interrupt
                //  - Adonis: do it after fnPrepareHw() (HW is initialized, otherwise IntPolarity may be wrong!)
            {
                // set trace by returned ErrorString
                fnSetTraceFctError( pDevExtIn,
                                    sError);
            }
            else
            {
                // LegIntConnected
                pDevExtIn->bLegIntConnected = TRUE;

                // success
                bResult = TRUE;
            }
        #else

            // no LegIntConnected
            pDevExtIn->bLegIntConnected = FALSE;

            // success
            bResult = TRUE;

        #endif
    }

    return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN fnEvtDevicePrepareHw(DEVICE_EXTENSION* pDevExtIn)
{
BOOLEAN bResult         = FALSE;
_TCHAR  sError[PNDEV_SIZE_STRING_BUF] = {0};

    if  (!fnPrepareHw(  pDevExtIn,
                        NULL,
                        ePNDEV_RESET_ACTION_REINIT_HW,
                        _countof(sError),
                        sError))
        // error at preparing hardware
    {
        // set trace by returned ErrorString
        fnSetTraceFctError( pDevExtIn,
                            sError);
    }
    else
    {
        #if (PNDEV_CONNECT_LEG_INT == YES)

            if  (!fnConnectInt( pDevExtIn,
                                _countof(sError),
                                sError))
                // error at connecting interrupt
                //  - Adonis: do it after fnPrepareHw() (HW is initialized, otherwise IntPolarity may be wrong!)
            {
                // set trace by returned ErrorString
                fnSetTraceFctError( pDevExtIn,
                                    sError);
            }
            else
            {
                // LegIntConnected
                pDevExtIn->bLegIntConnected = TRUE;

                // success
                bResult = TRUE;
            }
        #else

            // no LegIntConnected
            pDevExtIn->bLegIntConnected = FALSE;

            // success
            bResult = TRUE;

        #endif
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
//  CBF for releasing hardware
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
// Note:
//  - only be called when the device stopped for resource re-balance, surprise-removed or query-removed
//  - if failure: device stack will be torn down
//  - called at PASSIVE_LEVEL

NTSTATUS fnEvtDeviceReleaseHw(  IN WDFDEVICE    hDeviceIn,
                                IN WDFCMRESLIST hResTranslatedIn)           // translated PnP resources associated with the device. This is what is important to a PCI device
{
NTSTATUS            lResult = STATUS_SUCCESS;
DEVICE_EXTENSION*   pDevExt = NULL;
_TCHAR              sError[PNDEV_SIZE_STRING_BUF] = {0};

    // Note:
    //  - unmap the resources that were mapped in fnEvtDevicePrepareHw

    UNREFERENCED_PARAMETER(hResTranslatedIn);

    PAGED_CODE();

    pDevExt = fnGetDeviceContext(hDeviceIn);

    if  (!fnReleaseHw(  pDevExt,
                        _countof(sError),
                        sError))
        // error at releasing hardware
    {
        // set trace by returned ErrorString
        fnSetTraceFctError( pDevExt,
                            sError);
    }
    else
    {
        // success
        lResult = STATUS_SUCCESS;
    }

    return(lResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
BOOLEAN fnEvtDeviceReleaseHw(DEVICE_EXTENSION* pDevExtIn)
{
BOOLEAN bResult = FALSE;
_TCHAR  sError[PNDEV_SIZE_STRING_BUF] = {0};

    if  (!fnReleaseHw(  pDevExtIn,
                        _countof(sError),
                        sError))
        // error at releasing hardware
    {
        // set trace by returned ErrorString
        fnSetTraceFctError( pDevExtIn,
                            sError);
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
BOOLEAN fnEvtDeviceReleaseHw(DEVICE_EXTENSION* pDevExtIn)
{
BOOLEAN bResult = FALSE;
_TCHAR  sError[PNDEV_SIZE_STRING_BUF] = {0};

    if  (!fnReleaseHw(  pDevExtIn,
                        _countof(sError),
                        sError))
        // error at releasing hardware
    {
        // set trace by returned ErrorString
        fnSetTraceFctError( pDevExtIn,
                            sError);
    }
    else
    {
        // success
        bResult = TRUE;

        // undo pci init
        {
            // release regions
            pci_release_regions(pDevExtIn->pPciDev);

            // disable device
            pci_disable_device(pDevExtIn->pPciDev);
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
//  CBF for IoControl / IoDeviceControl
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
void fnEvtIoctl(WDFQUEUE    hQueueIn,                                       // handle to the framework queue object that is associated with the I/O request
                WDFREQUEST  hReqIn,                                         // handle to a framework request object
                size_t      lSizeOutputBufIn,                               // length of the request's output buffer, if an output buffer is available
                size_t      lSizeInputBufIn,                                // length of the request's input buffer, if an input buffer is available
                ULONG       lIoctlIn)                                       // driver-defined or system-defined I/O control code (IOCTL) that is associated with the request
{
NTSTATUS            lResult         = STATUS_UNSUCCESSFUL;
ePNDEV_IOCTL        eResultIoctl    = ePNDEV_IOCTL_INVALID;
DEVICE_EXTENSION*   pDevExt         = NULL;
PIRP                pIrp            = NULL;

    // Note:
    //  - called when the framework receives IRP_MJ_DEVICE_CONTROL requests from the system

    // get DeviceExtension
    pDevExt = fnGetDeviceContext(WdfIoQueueGetDevice(hQueueIn));

    // get IRP
    pIrp = WdfRequestWdmGetIrp(hReqIn);

    // do IoControl
    eResultIoctl = fnDoIoctl(   pDevExt,
                                hReqIn,
                                lIoctlIn,
                                lSizeInputBufIn,
                                lSizeOutputBufIn,
                                NULL,               // InputBuf  must be retrieved by hReqIn
                                NULL);              // OutputBuf must be retrieved by hReqIn

    // Note:
    //  - errors are returned by pOut->sError if possible
    //  - otherwise an error must be returned by ReturnValue (e.g. buffer could not be retrieved)

    switch  (eResultIoctl)
            // result
    {
        case ePNDEV_IOCTL_OK:               {lResult = STATUS_SUCCESS;                  break;}
        case ePNDEV_IOCTL_PENDING:          {lResult = STATUS_PENDING;                  break;}
        case ePNDEV_IOCTL_ERROR_BY_STRING:  {lResult = STATUS_SUCCESS;                  break;}     // errors are returned by pOut->sError
        case ePNDEV_IOCTL_ERROR_REQ:        {lResult = STATUS_INVALID_DEVICE_REQUEST;   break;}
        case ePNDEV_IOCTL_ERROR_BUF_SIZE:   {lResult = STATUS_INVALID_BUFFER_SIZE;      break;}
        case ePNDEV_IOCTL_ERROR_BUF_IN:     {lResult = STATUS_INVALID_USER_BUFFER;      break;}
        case ePNDEV_IOCTL_ERROR_BUF_OUT:    {lResult = STATUS_INVALID_USER_BUFFER;      break;}
        default:
        {
            break;
        }
    }

    if  (lResult != STATUS_PENDING)
        // request not pending
    {
        // complete request
        WdfRequestCompleteWithInformation(  hReqIn,
                                            lResult,
                                            lSizeOutputBufIn);
    }
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
// CBF for ioctl of driver (see adn_drv_register())

int32_t fnEvtIoctl( adn_drv_info_t* uInfoIn,
                    int             lCmdIn,
                    void*           pArgIn)
{
int32_t lResult = ADN_ERROR;
_TCHAR  sError[PNDEV_SIZE_STRING_BUF]           = {0};
char    sTraceDetail[PNDEV_SIZE_STRING_TRACE]   = {0};

    if  (   (lCmdIn == IOCTL_EVENT_CREATE)
        ||  (lCmdIn == IOCTL_EVENT_DELETE))
        // event created/deleted
    {
        if  (pthread_mutex_lock(&g_uVarSys.uCritSecIoctlDriver) != ADN_OK)
            // error at locking DriverMutex
        {
            // set TraceDetail
            RtlStringCchPrintfA(sTraceDetail,
                                _countof(sTraceDetail),
                                "PnDev[x,x,x]: ########## Error at locking DriverMutex!");

            // set trace
            fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                        ePNDEV_TRACE_CONTEXT_BASIC,
                        TRUE,                       // logging
                        sTraceDetail);
        }
        else
        {
        BOOLEAN bResultEvent = FALSE;

            if  (lCmdIn == IOCTL_EVENT_CREATE)
                // event created
            {
            UINT32              lEventId        = 0;
            adn_evt_event_t*    pEventObject    = NULL;

                // success
                lResult = ADN_OK;

                // get EventObject of Adonis
                pEventObject = (adn_evt_event_t*) pArgIn;

                // get EventId allocated by IOCTL_ALLOC_EVENT_ID
                lEventId = pEventObject->eventid;

                // save EventObject at EventHandleTable
                g_uVarSys.uEventHandleTable[lEventId].pEventObject = pArgIn;
            }
            else    // event deleted
            {
                // success
                bResultEvent = TRUE;
            }

            // Note:
            //  - after a successful lock of DriverMutex an unlocking must be done anyway!

            if  (pthread_mutex_unlock(&g_uVarSys.uCritSecIoctlDriver) != ADN_OK)
                // error at unlocking DriverMutex
            {
                // set TraceDetail
                RtlStringCchPrintfA(sTraceDetail,
                                    _countof(sTraceDetail),
                                    "PnDev[x,x,x]: ########## Error at unlocking DriverMutex!");

                // set trace
                fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                            ePNDEV_TRACE_CONTEXT_BASIC,
                            TRUE,                       // logging
                            sTraceDetail);
            }
            else
            {
                if  (bResultEvent)
                    // success
                {
                    lResult = ADN_OK;
                }
            }
        }
    }
    else
    {
    uPNDEV_IOCTL_ARG*   pIoctlArg       = NULL;
    ePNDEV_IOCTL        eResultIoctl    = ePNDEV_IOCTL_INVALID;

        // get IoctlArg
        pIoctlArg = (uPNDEV_IOCTL_ARG*) pArgIn;

        if  (pIoctlArg == NULL)
            // invalid ptr
        {
            // set TraceDetail
            RtlStringCchPrintfA(sTraceDetail,
                                _countof(sTraceDetail),
                                "PnDev[x,x,x]: ########## Invalid IoctlArg!");

            // set trace
            fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                        ePNDEV_TRACE_CONTEXT_BASIC,
                        TRUE,                       // logging
                        sTraceDetail);
        }
        else
        {
        uPNDEV_IOCTL_OUT* pOutCommon = NULL;

            // success
            lResult = ADN_OK;

            pOutCommon = (uPNDEV_IOCTL_OUT*) pIoctlArg->uOut.pPtr;

            // do IoControl
            eResultIoctl = fnDoIoctl(   pIoctlArg->uDeviceSys.hHandle,  // pDevExt
                                        NULL,                           // no OsPar
                                        lCmdIn,                         // Ioctl
                                        pIoctlArg->lSizeInputBuf,
                                        pIoctlArg->lSizeOutputBuf,
                                        pIoctlArg->uIn.pPtr,
                                        pIoctlArg->uOut.pPtr);

            if  (eResultIoctl == ePNDEV_IOCTL_PENDING)
                // request pending
                //  - request should be stored for a later confirmation
            {
                eResultIoctl = ePNDEV_IOCTL_ERROR_BY_STRING;

                // set ErrorString
                fnBuildString(  NULL,
                                _TEXT("Invalid UseCase (request pending)!"),
                                NULL,
                                _countof(pOutCommon->sError),
                                pOutCommon->sError);
            }

            switch  (eResultIoctl)
                    // result
            {
                case ePNDEV_IOCTL_OK:
                case ePNDEV_IOCTL_ERROR_BY_STRING:
                {
                    // nothing to do

                    break;
                }
                case ePNDEV_IOCTL_ERROR_REQ:
                case ePNDEV_IOCTL_ERROR_BUF_SIZE:
                case ePNDEV_IOCTL_ERROR_BUF_IN:
                case ePNDEV_IOCTL_ERROR_BUF_OUT:
                default:
                {
                _TCHAR sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

                    // set ErrorDetail
                    _RtlStringCchPrintf(sErrorDetail,
                                        _countof(sErrorDetail),
                                        _TEXT("(): Error=0x%x!"),
                                        (UINT32) eResultIoctl);

                    // set ErrorString
                    fnBuildString(  _TEXT(__FUNCTION__),
                                    sErrorDetail,
                                    NULL,
                                    _countof(pOutCommon->sError),
                                    pOutCommon->sError);

                    break;
                }
            }
        }
    }

    return(lResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
long fnEvtIoctl (	struct file* pFdIn,
                 	unsigned int lCmdIn,
                 	unsigned long lArgIn)
{
long                lResult         = LINUX_ERROR;
BOOLEAN             bResultTmp      = FALSE;
uPNDEV_IOCTL_ARG    uIoctlArg;
uPNDEV64_PTR_VOID   uInBuffer;
uPNDEV64_PTR_VOID   uOutBuffer;
char sTraceDetail[PNDEV_SIZE_STRING_TRACE]  = {0};

        // retrieve ioctl buffer
        bResultTmp = RetrieveIoctlBuffer(   lCmdIn,
                                            lArgIn,
                                            &uIoctlArg,
                                            &uInBuffer,
                                            &uOutBuffer);

        if  (bResultTmp == FALSE)
            // error
        {
            // set TraceDetail
            RtlStringCchPrintfA(sTraceDetail,
                                _countof(sTraceDetail),
                                "PnDev[x,x,x]: ########## Error at RetrieveIoctlBuffer!");

            // set trace
            fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                        ePNDEV_TRACE_CONTEXT_BASIC,
                        TRUE,                       // logging
                        sTraceDetail);
        }
        else
        {
            uPNDEV_IOCTL_OUT*   pOutCommon      = NULL;
            ePNDEV_IOCTL        eResultIoctl    = ePNDEV_IOCTL_INVALID;

            pOutCommon = (uPNDEV_IOCTL_OUT*) uOutBuffer.pPtr;

            // do IoControl
            eResultIoctl = fnDoIoctl(   uIoctlArg.uDeviceSys.hHandle,   // pDevExt
                                        NULL,                           // no OsPar
                                        lCmdIn,                         // Ioctl
                                        uIoctlArg.lSizeInputBuf,
                                        uIoctlArg.lSizeOutputBuf,
                                        uInBuffer.pPtr,
                                        uOutBuffer.pPtr);

            if  (eResultIoctl == ePNDEV_IOCTL_PENDING)
                // request pending
                //  - request should be stored for a later confirmation
            {
                eResultIoctl = ePNDEV_IOCTL_ERROR_BY_STRING;

                // set ErrorString
                fnBuildString(  NULL,
                                _TEXT("Invalid UseCase (request pending)!"),
                                NULL,
                                _countof(pOutCommon->sError),
                                pOutCommon->sError);
            }

            switch  (eResultIoctl)
                    // result
            {
                case ePNDEV_IOCTL_OK:
                case ePNDEV_IOCTL_ERROR_BY_STRING:
                {
                    // nothing to do

                    break;
                }
                case ePNDEV_IOCTL_ERROR_REQ:
                case ePNDEV_IOCTL_ERROR_BUF_SIZE:
                case ePNDEV_IOCTL_ERROR_BUF_IN:
                case ePNDEV_IOCTL_ERROR_BUF_OUT:
                default:
                {
                _TCHAR sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

                    // set ErrorDetail
                    _RtlStringCchPrintf(sErrorDetail,
                                        _countof(sErrorDetail),
                                        _TEXT("(): Error=0x%x!"),
                                        (UINT32) eResultIoctl);

                    // set ErrorString
                    fnBuildString(  _TEXT(__FUNCTION__),
                                    sErrorDetail,
                                    NULL,
                                    _countof(pOutCommon->sError),
                                    pOutCommon->sError);

                    break;
                }
            }

            // return ioctl buffer
            bResultTmp = ReturnIoctlBuffer( &uIoctlArg,
                                            &uInBuffer,
                                            &uOutBuffer);

            if  (bResultTmp == FALSE)
                // error
            {
                // set TraceDetail
                RtlStringCchPrintfA(sTraceDetail,
                                    _countof(sTraceDetail),
                                    "PnDev[x,x,x]: ########## Error at RetrieveIoctlBuffer!");

                // set trace
                fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                            ePNDEV_TRACE_CONTEXT_BASIC,
                            TRUE,                       // logging
                            sTraceDetail);
            }
            else
            {
                // success
                lResult = LINUX_OK;
            }
        }

    return(lResult);
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  CBF for ISR
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN fnEvtIsr(   IN WDFINTERRUPT hInterruptIn,
                    IN ULONG        lMsgIdMsiIn)
{
BOOLEAN             bResult     = FALSE;
DEVICE_EXTENSION*   pDevExt     = NULL;

    pDevExt = fnGetDeviceContext(WdfInterruptGetDevice(hInterruptIn));

    // do ISR
    //  - ReturnValue indicates if interrupt is caused by this device
    bResult = fnIsr(pDevExt,
                    hInterruptIn,
                    lMsgIdMsiIn);

    return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------

void fnEvtIsr(  int     lMsgIdMsiIn,
                void*   pFctArgIn)
{
DEVICE_EXTENSION* pDevExt = (DEVICE_EXTENSION*) pFctArgIn;

    // increase performance for adonis
    // if TopLevel-ICU, call Evnet immediately

    if  (pDevExt->eIcu == ePNDEV_ICU_TOP_LEVEL_PCI)
        // TopLevel-ICU = PCI-ICU
    {
        if  (pDevExt->bResetRunning)
            // reset running
        {
            // don't access Asic while reset is running
            //  - after disabling interrupts an interrupt may be stored at APIC or uP
        }
        else
        {
        char sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

            if  (pDevExt->bIsrRunning)
                // ISR interrupted itself
            {
                // set FatalAsync
                pDevExt->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__ERROR_ISR_LOCK;

                // set TraceDetail
                RtlStringCchPrintfA(sTraceDetail,
                                    _countof(sTraceDetail),
                                    "PnDev[%s]: ########## ISR lock error!",
                                    pDevExt->sPciLocShortAscii);

                // set trace
                fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                            ePNDEV_TRACE_CONTEXT_ISR,
                            TRUE,                       // logging
                            sTraceDetail);
            }
            else
            {
                // set state IsrRunning
                pDevExt->bIsrRunning = TRUE;
                {
                UINT32 lIrVec = 0;

                    #ifdef PNDEV_TRACE_ISR_DPC
                        // check if blank interrupt
                        {
                            // read PCI-ICU-MuxA-Vector
                            lIrVec = READ_REG_UINT32(pDevExt->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_IRVEC_A);

                            if  (lIrVec == 0)
                                // no PCI-ICU-MuxA event set (0=DefaultVector)
                            {
                                // set TraceDetail
                                RtlStringCchPrintfA(sTraceDetail,
                                                    _countof(sTraceDetail),
                                                    "PnDev[%s]: ########## Blank interrupt!",
                                                    pDevExt->sPciLocShortAscii);

                                // set trace
                                fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                                            ePNDEV_TRACE_CONTEXT_ISR,
                                            TRUE,                       // logging
                                            sTraceDetail);
                            }
                        }
                    #endif

                    // disable all events
                    WRITE_REG_UINT32(   (pDevExt->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_MASKALL_A),
                                        1);

                    // ensure that last PCI-Write is done (read on same IP)
                    //  - otherwise IntLine may stay active after end of ISR -> ISR is called again!
                    //  - read PCI-ICU-MuxA-Vector (must be 0=DefaultVector if interrupts are masked)
                    lIrVec = READ_REG_UINT32(pDevExt->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_IRVEC_A);

                    if  (lIrVec != 0)
                        // error
                    {
                        // set FatalAsync
                        pDevExt->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__ERROR_INT_MASK;

                        // set TraceDetail
                        RtlStringCchPrintfA(sTraceDetail,
                                            _countof(sTraceDetail),
                                            "PnDev[%s]: ########## Interrupt mask error!",
                                            pDevExt->sPciLocShortAscii);

                        // set trace
                        fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                                    ePNDEV_TRACE_CONTEXT_ISR,
                                    TRUE,                       // logging
                                    sTraceDetail);
                    }
                    else
                    {
                        // set EventUisr of AppObject (MasterApp)
                        adn_evt_fire_event((adn_evt_event_t*) pDevExt->pMasterApp->hEventUisr);

                        // AsicEoi need not to be set because ISREG is not used
                    }
                }
                // set restate IsrRunning
                pDevExt->bIsrRunning = FALSE;
            }
        }
    }
    else
    {
        // do ISR
        fnIsr(  pDevExt,
                NULL,
                lMsgIdMsiIn);
    }
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
irqreturn_t fnEvtIsr(   int    lMsgIdMsiIn,
                        void*  pFctArgIn)
{
DEVICE_EXTENSION* pDevExt = (DEVICE_EXTENSION*) pFctArgIn;

    // increase performance for adonis
    // if TopLevel-ICU, call Evnet immediately
    if  (pDevExt->eIcu == ePNDEV_ICU_TOP_LEVEL_PCI)
        // TopLevel-ICU = PCI-ICU
    {
        if  (pDevExt->bResetRunning)
            // reset running
        {
            // don't access Asic while reset is running
            //  - after disabling interrupts an interrupt may be stored at APIC or uP
        }
        else
        {
        char sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

            if  (pDevExt->bIsrRunning)
                // ISR interrupted itself
            {
                // set FatalAsync
                pDevExt->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__ERROR_ISR_LOCK;

                // set TraceDetail
                RtlStringCchPrintfA(sTraceDetail,
                                    _countof(sTraceDetail),
                                    "PnDev[%s]: ########## ISR lock error!",
                                    pDevExt->sPciLocShortAscii);

                // set trace
                fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                            ePNDEV_TRACE_CONTEXT_ISR,
                            TRUE,                       // logging
                            sTraceDetail);
            }
            else
            {
                // set state IsrRunning
                pDevExt->bIsrRunning = TRUE;
                {
                UINT32 lIrVec = 0;

                    #ifdef PNDEV_TRACE_ISR_DPC
                        // check if blank interrupt
                        {
                            // read PCI-ICU-MuxA-Vector
                            lIrVec = READ_REG_UINT32(pDevExt->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_IRVEC_A);

                            if  (lIrVec == 0)
                                // no PCI-ICU-MuxA event set (0=DefaultVector)
                            {
                                // set TraceDetail
                                RtlStringCchPrintfA(sTraceDetail,
                                                    _countof(sTraceDetail),
                                                    "PnDev[%s]: ########## Blank interrupt!",
                                                    pDevExt->sPciLocShortAscii);

                                // set trace
                                fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                                            ePNDEV_TRACE_CONTEXT_ISR,
                                            TRUE,                       // logging
                                            sTraceDetail);
                            }
                        }
                    #endif

                    // disable all events
                    WRITE_REG_UINT32(   (pDevExt->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_MASKALL_A),
                                        1);

                    // ensure that last PCI-Write is done (read on same IP)
                    //  - otherwise IntLine may stay active after end of ISR -> ISR is called again!
                    //  - read PCI-ICU-MuxA-Vector (must be 0=DefaultVector if interrupts are masked)
                    lIrVec = READ_REG_UINT32(pDevExt->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_IRVEC_A);

                    if  (lIrVec != 0)
                        // error
                    {
                        // set FatalAsync
                        pDevExt->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__ERROR_INT_MASK;

                        // set TraceDetail
                        RtlStringCchPrintfA(sTraceDetail,
                                            _countof(sTraceDetail),
                                            "PnDev[%s]: ########## Interrupt mask error!",
                                            pDevExt->sPciLocShortAscii);

                        // set trace
                        fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                                    ePNDEV_TRACE_CONTEXT_ISR,
                                    TRUE,                       // logging
                                    sTraceDetail);
                    }
                    else
                    {
                    struct eventfd_ctx* pEventfdCtx     = NULL;     // eventfd context

                        
                        if ( ( pDevExt->pMasterApp == NULL) || ( pDevExt->pMasterApp->hEventUisr == NULL))
                            // error
                        {

                            // set ErrorString
                            RtlStringCchPrintfA(sTraceDetail,
                                                _countof(sTraceDetail),
                                                "PnDev[%s]: ########## Invalid handle in fnEvtIsr for eventfd_ctx_fileget!",
                                                pDevExt->sPciLocShortAscii);
                            
                            fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                                        ePNDEV_TRACE_CONTEXT_ISR,
                                        TRUE,                       // logging
                                        sTraceDetail);
                        }
                        else
                        {
                            // set EventUisr of AppObject (MasterApp)
                            pEventfdCtx = eventfd_ctx_fileget((struct file*) pDevExt->pMasterApp->hEventUisr);
                        }

                        if  (pEventfdCtx != NULL)
                            // Successfully resolved eventfd context
                        {
                            // z002p64u: eventfd_ctx_fileget and eventfd_signal not tested yet in interrupt Todo!
                            // Increment userspace program's eventfd's counter by eEventIn
                            eventfd_signal( pEventfdCtx,
                                            ePNDEV_EVENT_THREAD_DLL_UISR);
                            eventfd_ctx_put(pEventfdCtx);
                        }
                        else
                        {
                            // set FatalAsync
                            pDevExt->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__ERROR_EVENT;

                            // set TraceDetail
                            RtlStringCchPrintfA(sTraceDetail,
                                                _countof(sTraceDetail),
                                                "PnDev[%s]: ########## Error at eventfd_ctx_fileget() in fnEvtIsr!",
                                                pDevExt->sPciLocShortAscii);

                            // set trace
                            fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                                        ePNDEV_TRACE_CONTEXT_ISR,
                                        TRUE,                       // logging
                                        sTraceDetail);
                        }

                        // AsicEoi need not to be set because ISREG is not used
                    }
                }
                // set restate IsrRunning
                pDevExt->bIsrRunning = FALSE;
            }
        }
    }
    else
    {
        // do ISR
        fnIsr(  pDevExt,
                NULL,
                lMsgIdMsiIn);
    }

    return IRQ_HANDLED;
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  CBF for DPC (deferred procedure call)
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
void fnEvtDpc(  IN WDFINTERRUPT hInterruptIn,                               // handle to WDFINTERRUPT Object for this device
                IN WDFDEVICE    hDeviceIn)                                  // WDFDEVICE object passed to WdfInterruptCreate()
{
DEVICE_EXTENSION* pDevExt = NULL;

    // Note:
    //  - on a multiprocessor system you could have more than one DPCs running simultaneously on multiple processors
    //  - so if you are accessing any global resources make sure to synchronize the accesses with a SpinLock

    UNREFERENCED_PARAMETER(hDeviceIn);

    pDevExt = fnGetDeviceContext(WdfInterruptGetDevice(hInterruptIn));

    // do DPC
    fnDpc(pDevExt);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  CBF for TimerEvent
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
void fnEvtTimer(IN WDFTIMER hTimerIn)                                       // handle to WDFTIMER Object for this device
{
DEVICE_EXTENSION*   pDevExt     = NULL;
KIRQL               lIrqlCur    = PASSIVE_LEVEL;

    pDevExt = fnGetDeviceContext(WdfTimerGetParentObject(hTimerIn));

    if  (!fnCheckIrql(  DISPATCH_LEVEL,
                        &lIrqlCur))
        // invalid IRQL
    {
    char sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

        // set TraceDetail
        RtlStringCchPrintfA(sTraceDetail,
                            _countof(sTraceDetail),
                            "PnDev[%s]: ########## Invalid Irql (=%u) [%s()]!",
                            pDevExt->sPciLocShortAscii,
                            lIrqlCur,
                            __FUNCTION__);

        // set trace
        fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                    ePNDEV_TRACE_CONTEXT_DPC,
                    TRUE,                       // logging
                    sTraceDetail);
    }
    else
    {
        //------------------------------------------------------------------------
        // acquire DpcSpinLock
        //  - protect against re-entrance at MultiCore system!
        fnAcquireSpinLock(  pDevExt,
                            ePNDEV_SPIN_LOCK_DPC);
        {
        }
        //------------------------------------------------------------------------
        // release DpcSpinLock
        fnReleaseSpinLock(  pDevExt,
                            ePNDEV_SPIN_LOCK_DPC);
    }
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  enable interrupt
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
// Note:
//  - called by the framework at DIRQL immediately after registering the ISR with the kernel by calling IoConnectInterrupt

NTSTATUS fnEvtIntEnable(IN WDFINTERRUPT hInterruptIn,
                        IN WDFDEVICE    hDeviceIn)
{
NTSTATUS            lResult = STATUS_UNSUCCESSFUL;
DEVICE_EXTENSION*   pDevExt = NULL;

    pDevExt = fnGetDeviceContext(WdfInterruptGetDevice(hInterruptIn));

    lResult = STATUS_SUCCESS;

    return(lResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  disable interrupt
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
// Note:
//  - called by the framework at DIRQL before deregistering the ISR with the kernel by calling IoDisconnectInterrupt

NTSTATUS fnEvtIntDisable(   IN WDFINTERRUPT hInterruptIn,
                            IN WDFDEVICE    hDeviceIn)
{
NTSTATUS            lResult = STATUS_UNSUCCESSFUL;
DEVICE_EXTENSION*   pDevExt = NULL;

    pDevExt = fnGetDeviceContext(WdfInterruptGetDevice(hInterruptIn));

    lResult = STATUS_SUCCESS;

    return(lResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  SysThread
//  - each device has its own thread
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
void fnThreadSys(void* pThreadParIn)
{
DEVICE_EXTENSION*   pDevExt = NULL;
BOOLEAN             bStop   = FALSE;
LARGE_INTEGER       uTimeOutEvent;
char                sTraceDetail[PNDEV_SIZE_STRING_TRACE]   = {0};

    pDevExt = (DEVICE_EXTENSION*) pThreadParIn;
/*
    // set ThreadPriority
    {
    PKTHREAD pThread = NULL;

        pThread = KeGetCurrentThread();

        KeSetPriorityThread(pThread,
                            HIGH_PRIORITY);
    }
*/

    // Note:
    //  - CyclicTimer must be implemented by a thread because of synchronization issues
    //  - SleepTime is done by timeout of KeWaitForSingleObject()

    // set SleepTime of SysThread
    //  - timeout in 100nsec (10msec=10*1000*10 * 100nsec, negative: relative time)
    //  - cast to INT32 necessary because of negative value!
    uTimeOutEvent.QuadPart = (-100000) * ((INT32) pDevExt->lTimeCycleThreadSys_10msec);

    // set TraceDetail
    RtlStringCchPrintfA(sTraceDetail,
                        _countof(sTraceDetail),
                        "PnDev[%s]: SysThread started",
                        pDevExt->sPciLocShortAscii);

    // set trace
    fnSetTrace( ePNDEV_TRACE_LEVEL_INFO,
                ePNDEV_TRACE_CONTEXT_BASIC,
                TRUE,                       // logging
                sTraceDetail);

    do
    {
    NTSTATUS        lResultEvent = STATUS_UNSUCCESSFUL;
    uPNDEV_CLOCK    uClockStart;

        // do actions of SysThread
        fnDoThreadSys(  pDevExt,
                        pDevExt->lTimeCycleThreadSys_10msec);

        #ifdef PNDEV_GET_RUNTIME_THREAD
            // get clock
            fnGetClock(&uClockStart);
        #endif

        // check EventThreadStop
        //  - SleepTime of SysThread is implemented by timeout
        lResultEvent = KeWaitForSingleObject(   &pDevExt->uEventThreadStop,
                                                Executive,
                                                KernelMode,
                                                FALSE,
                                                &uTimeOutEvent);

        if  (lResultEvent == STATUS_SUCCESS)
            // EventThreadStop set
        {
            bStop = TRUE;
        }

        #ifdef PNDEV_GET_RUNTIME_THREAD
        {
        UINT32 lRuntime_Usec = 0;

            // compute Runtime in usec
            lRuntime_Usec = fnComputeRunTimeUsec(&uClockStart);

            // set TraceDetail
            RtlStringCchPrintfA(sTraceDetail,
                                _countof(sTraceDetail),
                                "PnDev[%s]: ---- Thread CycleTime=%u usec",
                                pDevExt->sPciLocShortAscii,
                                lRuntime_Usec);

            // set trace
            fnSetTrace( ePNDEV_TRACE_LEVEL_INFO,
                        ePNDEV_TRACE_CONTEXT_BASIC,
                        FALSE,                      // logging
                        sTraceDetail);
        }
        #endif
    }
    while   (!bStop);
            // thread not stopped

    // set TraceDetail
    RtlStringCchPrintfA(sTraceDetail,
                        _countof(sTraceDetail),
                        "PnDev[%s]: SysThread stopped",
                        pDevExt->sPciLocShortAscii);

    // set trace
    fnSetTrace( ePNDEV_TRACE_LEVEL_INFO,
                ePNDEV_TRACE_CONTEXT_BASIC,
                TRUE,                       // logging
                sTraceDetail);

    // terminate SysThread
    //  - can only be called by thread itself
    PsTerminateSystemThread(STATUS_SUCCESS);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
#ifdef PNDEV_GET_RUNTIME_THREAD
    UINT32          g_lCtrEntry = 0;
    uPNDEV_CLOCK    g_uClockStart;
    UINT32          g_lArrayRuntime_Usec[10] = {0,0,0,0,0,0,0,0,0,0};
#endif

void fnThreadSys(union sigval uThreadParIn)
{
DEVICE_EXTENSION* pDevExt = NULL;

    pDevExt = (DEVICE_EXTENSION*) uThreadParIn.sival_ptr;

    #ifdef PNDEV_GET_RUNTIME_THREAD
    {
        if  (g_lCtrEntry == 0)
            // first entry
        {
            // get clock
            fnGetClock(&g_uClockStart);
        }
        else if (g_lCtrEntry < 10)
        {
        UINT32 lRuntime_Usec = 0;

            // compute Runtime in usec
            lRuntime_Usec = fnComputeRunTimeUsec(&g_uClockStart);

            g_lArrayRuntime_Usec[g_lCtrEntry] = lRuntime_Usec;
        }
        else
        {
            g_lCtrEntry--;
            g_lCtrEntry++;
        }

        g_lCtrEntry++;
    }
    #endif

    // do actions of SysThread
    fnDoThreadSys(  pDevExt,
                    pDevExt->lTimeCycleThreadSys_10msec);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
#ifdef PNDEV_GET_RUNTIME_THREAD
    UINT32          g_lCtrEntry = 0;
    uPNDEV_CLOCK    g_uClockStart;
    UINT32          g_lArrayRuntime_Usec[10] = {0,0,0,0,0,0,0,0,0,0};
#endif

void fnThreadSys(unsigned long arg)
{
UINT32  lTimeCycle_Sec = 0;
UINT32  lTimeCycle_Msec = 0;
DEVICE_EXTENSION* pDevExt = (DEVICE_EXTENSION*) arg;

    if  (!pDevExt->bSysThreadTimerStop)
        // don't TimerStop
    {
            //calculate CycleTime in sec/nsec
            lTimeCycle_Sec  = (pDevExt->lTimeCycleThreadSys_10msec / 100);
            lTimeCycle_Msec = (pDevExt->lTimeCycleThreadSys_10msec % 100) * 10; // *10 -> msec,...

            //set timer to next expirationtime
            mod_timer(&pDevExt->uSysThreadTimer,
                      jiffies + (HZ * lTimeCycle_Sec) + (HZ * lTimeCycle_Msec)/1000); // restarting timer


        #ifdef PNDEV_GET_RUNTIME_THREAD
        {
            if  (g_lCtrEntry == 0)
                // first entry
            {
                // get clock
                fnGetClock(&g_uClockStart);
            }
            else if (g_lCtrEntry < 10)
            {
            UINT32 lRuntime_Usec = 0;

                // compute Runtime in usec
                lRuntime_Usec = fnComputeRunTimeUsec(&g_uClockStart);

                g_lArrayRuntime_Usec[g_lCtrEntry] = lRuntime_Usec;
            }
            else
            {
                g_lCtrEntry--;
                g_lCtrEntry++;
            }

            g_lCtrEntry++;
        }
        #endif

        // do actions of SysThread
        fnDoThreadSys(pDevExt,
                      pDevExt->lTimeCycleThreadSys_10msec);
    }
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  start up resources for locking
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN fnStartUpLockRes(   DEVICE_EXTENSION*   pDevExtIn,
                            const UINT32        lDimErrorIn,
                            _TCHAR*             sErrorOut)
{
BOOLEAN                 bResult         = FALSE;
NTSTATUS                lResultCritSec1 = STATUS_UNSUCCESSFUL;
NTSTATUS                lResultCritSec2 = STATUS_UNSUCCESSFUL;
NTSTATUS                lResultCritSec3 = STATUS_UNSUCCESSFUL;
_TCHAR                  sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};
WDF_OBJECT_ATTRIBUTES   uAttributes;

    // Note:
    //  - the framework deletes a WaitLock object when it deletes the ParentObject
    //  - by default the parent is the framework DriverObject created by WdfDriverCreate()
    //      -> a WaitLock/SpinLock object would remain until the I/O manager unloads driver
    //  - this can be changed

    // WaitLock/SpinLock memory will be released automatically by the framework as part of the DeviceObject cleanup
    WDF_OBJECT_ATTRIBUTES_INIT(&uAttributes);
    uAttributes.ParentObject = pDevExtIn->hDeviceOs;

    // create CriticalSection (WaitLock object)
    lResultCritSec1 = WdfWaitLockCreate(&uAttributes,
                                        &pDevExtIn->hCritSecIoctl);
    lResultCritSec2 = WdfWaitLockCreate(&uAttributes,
                                        &pDevExtIn->hCritSecPage);
    lResultCritSec3 = WdfWaitLockCreate(&uAttributes,
                                        &pDevExtIn->hCritSecAppList);

    if  (   (lResultCritSec1 != STATUS_SUCCESS)
        ||  (lResultCritSec2 != STATUS_SUCCESS)
        ||  (lResultCritSec3 != STATUS_SUCCESS))
        // error
    {
        // set ErrorDetail
        _RtlStringCchPrintf(sErrorDetail,
                            _countof(sErrorDetail),
                            _TEXT("(): Error at WdfWaitLockCreate() (status1=%!STATUS!, status2=%!STATUS!, status3=%!STATUS!)!"),
                            lResultCritSec1,
                            lResultCritSec2,
                            lResultCritSec3);

        // set ErrorString
        fnBuildString(  _TEXT(__FUNCTION__),
                        sErrorDetail,
                        NULL,
                        lDimErrorIn,
                        sErrorOut);
    }
    else
    {
    NTSTATUS lResultSpinLock = STATUS_UNSUCCESSFUL;

        // create DpcSpinLock
        lResultSpinLock = WdfSpinLockCreate(&uAttributes,
                                            &pDevExtIn->hSpinLockDpc);

        if  (lResultSpinLock != STATUS_SUCCESS)
            // error
        {
            // set ErrorDetail
            _RtlStringCchPrintf(sErrorDetail,
                                _countof(sErrorDetail),
                                _TEXT("(): Error at WdfSpinLockCreate() (status=%!STATUS!)!"),
                                lResultSpinLock);

            // set ErrorString
            fnBuildString(  _TEXT(__FUNCTION__),
                            sErrorDetail,
                            NULL,
                            lDimErrorIn,
                            sErrorOut);
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
#elif defined (PNDEV_OS_ADONIS) || defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN fnStartUpLockRes(   DEVICE_EXTENSION*   pDevExtIn,
                            const UINT32        lDimErrorIn,
                            _TCHAR*             sErrorOut)
{
BOOLEAN bResult = FALSE;

    // Note:
    //  - DPC is directly called by ISR
    //      -> hSpinLockDpc not required

    if  (fnCreateMutex( lDimErrorIn,
                        &pDevExtIn->uCritSecIoctl,
                        sErrorOut))
        // creating DeviceMutex ok
    {
        if  (fnCreateMutex( lDimErrorIn,
                            &pDevExtIn->uCritSecPage,
                            sErrorOut))
            // creating DeviceMutex ok
        {
            if  (fnCreateMutex( lDimErrorIn,
                                &pDevExtIn->uCritSecAppList,
                                sErrorOut))
                // creating DeviceMutex ok
            {
//          int32_t lResultTmp = ADN_ERROR;
//
//              // initialize SpinLockIrq
//              lResultTmp = pthread_spin_init( &pDevExtIn->uSpinlockIrq,
//                                              ADN_DO_NOT_CARE);
//
//              if  (lResultTmp != ADN_OK)
//                  // error
//              {
//                  // set ErrorString
//                  fnBuildString(  _TEXT(__FUNCTION__),
//                                  _TEXT("(): Error at pthread_spin_init()!"),
//                                  NULL,
//                                  lDimErrorIn,
//                                  sErrorOut);
//              }
//              else
//              {
                    // success
                    bResult = TRUE;
//              }
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
//  shut down resources for locking
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN fnShutDownLockRes(  DEVICE_EXTENSION*   pDevExtIn,
                            const UINT32        lDimErrorIn,
                            _TCHAR*             sErrorOut)
{
BOOLEAN bResult = FALSE;

    // objects are automatically released, see fnStartUpLockRes()

    // success
    bResult = TRUE;

    return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS) || defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN fnShutDownLockRes(  DEVICE_EXTENSION*   pDevExtIn,
                            const UINT32        lDimErrorIn,
                            _TCHAR*             sErrorOut)
{
BOOLEAN bResult = FALSE;

    if  (fnDeleteMutex( &pDevExtIn->uCritSecIoctl,
                        lDimErrorIn,
                        sErrorOut))
        // deleting DeviceMutex ok
    {
        if  (fnDeleteMutex( &pDevExtIn->uCritSecPage,
                            lDimErrorIn,
                            sErrorOut))
            // deleting DeviceMutex ok
        {
            if  (fnDeleteMutex( &pDevExtIn->uCritSecAppList,
                                lDimErrorIn,
                                sErrorOut))
                // deleting DeviceMutex ok
            {
//              if  (pthread_spin_destroy(&pDevExtIn->uSpinlockIrq) != ADN_OK)
//                  // error at deleting SpinLockIrq
//              {
//                  // set ErrorString
//                  fnBuildString(  _TEXT(__FUNCTION__),
//                                  _TEXT("(): Error at pthread_spin_destroy()!"),
//                                  NULL,
//                                  lDimErrorIn,
//                                  sErrorOut);
//              }
//              else
//              {
                    // success
                    bResult = TRUE;
//              }
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
//  acquire SpinLockIrq
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
#pragma prefast(disable:28167, "IRQL is restored by another function")

void fnAcquireSpinLockIrq(DEVICE_EXTENSION* pDevExtIn)
{
    if  (pDevExtIn->hInterrupt != NULL)
        // valid ptr
    {
        WdfInterruptAcquireLock(pDevExtIn->hInterrupt);
    }
}
#pragma prefast(enable:28167, "IRQL is restored by another function")

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
void fnAcquireSpinLockIrq(DEVICE_EXTENSION* pDevExtIn)
{
    // disable IntLine at IO-APIC
    ADN_IRQ_DISABLE(pDevExtIn->lIntLine);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
void fnAcquireSpinLockIrq(DEVICE_EXTENSION* pDevExtIn)
{
    // disable IntLine at IO-APIC
    disable_irq_nosync(pDevExtIn->pPciDev->irq);
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  release SpinLockIrq
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
#pragma prefast(disable:28167, "IRQL is restored by another function")

void fnReleaseSpinLockIrq(DEVICE_EXTENSION* pDevExtIn)
{
    if  (pDevExtIn->hInterrupt != NULL)
        // valid ptr
    {
        WdfInterruptReleaseLock(pDevExtIn->hInterrupt);
    }
}
#pragma prefast(enable:28167, "IRQL is restored by another function")

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
void fnReleaseSpinLockIrq(DEVICE_EXTENSION* pDevExtIn)
{
    // enable IntLine at IO-APIC
    //  - clear its MaskBit
    ADN_IRQ_ENABLE(pDevExtIn->lIntLine);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
void fnReleaseSpinLockIrq(DEVICE_EXTENSION* pDevExtIn)
{
    enable_irq(pDevExtIn->pPciDev->irq);
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  acquire SpinLock
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
#pragma prefast(disable:28167, "IRQL is restored by another function")

void fnAcquireSpinLock( DEVICE_EXTENSION*   pDevExtIn,
                        ePNDEV_SPIN_LOCK    eSpinLockIn)
{
HANDLE hSpinLock = NULL;

    switch  (eSpinLockIn)
            // SpinLock
    {
        case ePNDEV_SPIN_LOCK_DPC:  {hSpinLock = pDevExtIn->hSpinLockDpc;   break;}
        default:
        {
        char sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

            // set TraceDetail
            RtlStringCchPrintfA(sTraceDetail,
                                _countof(sTraceDetail),
                                "PnDev[%s]: ########## Invalid SpinLock!",
                                pDevExtIn->sPciLocShortAscii);

            // set trace
            fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                        ePNDEV_TRACE_CONTEXT_BASIC,
                        TRUE,                       // logging
                        sTraceDetail);

            // set FatalAsync
            pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__ERROR_SPIN_LOCK;

            break;
        }
    }

    if  (hSpinLock != NULL)
        // success
    {
        // acquire SpinLock
        WdfSpinLockAcquire(hSpinLock);
    }
}
#pragma prefast(enable:28167, "IRQL is restored by another function")

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
void fnAcquireSpinLock( DEVICE_EXTENSION*   pDevExtIn,
                        ePNDEV_SPIN_LOCK    eSpinLockIn)
{
    // Note:
    //  - at Adonis this function has now effect
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
void fnAcquireSpinLock( DEVICE_EXTENSION*   pDevExtIn,
                        ePNDEV_SPIN_LOCK    eSpinLockIn)
{
    // Note:
    //  - at Linux this function has now effect
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  release SpinLock
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
#pragma prefast(disable:28167, "IRQL is restored by another function")

void fnReleaseSpinLock( DEVICE_EXTENSION*   pDevExtIn,
                        ePNDEV_SPIN_LOCK    eSpinLockIn)
{
HANDLE hSpinLock = NULL;

    switch  (eSpinLockIn)
            // SpinLock
    {
        case ePNDEV_SPIN_LOCK_DPC:  {hSpinLock = pDevExtIn->hSpinLockDpc;   break;}
        default:
        {
        char sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

            // set TraceDetail
            RtlStringCchPrintfA(sTraceDetail,
                                _countof(sTraceDetail),
                                "PnDev[%s]: ########## Invalid SpinLock!",
                                pDevExtIn->sPciLocShortAscii);

            // set trace
            fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                        ePNDEV_TRACE_CONTEXT_BASIC,
                        TRUE,                       // logging
                        sTraceDetail);

            // set FatalAsync
            pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__ERROR_SPIN_LOCK;

            break;
        }
    }

    if  (hSpinLock != NULL)
        // success
    {
        // release SpinLock
        WdfSpinLockRelease(hSpinLock);
    }
}
#pragma prefast(enable:28167, "IRQL is restored by another function")

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
void fnReleaseSpinLock( DEVICE_EXTENSION*   pDevExtIn,
                        ePNDEV_SPIN_LOCK    eSpinLockIn)
{
    // Note:
    //  - at Adonis this function will do nothing
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
void fnReleaseSpinLock( DEVICE_EXTENSION*   pDevExtIn,
                        ePNDEV_SPIN_LOCK    eSpinLockIn)
{
    // Note:
    //  - at Linux this function will do nothing
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  enter CriticalSection
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
void fnEnterCritSec(DEVICE_EXTENSION*   pDevExtIn,
                    ePNDEV_CRIT_SEC_SYS eCritSecIn)
{
HANDLE hCritSec = NULL;

    switch  (eCritSecIn)
            // CritSec
    {
        case ePNDEV_CRIT_SEC_SYS_IOCTL:     {hCritSec = pDevExtIn->hCritSecIoctl;   break;}
        case ePNDEV_CRIT_SEC_SYS_PAGE:      {hCritSec = pDevExtIn->hCritSecPage;    break;}
        case ePNDEV_CRIT_SEC_SYS_APP_LIST:  {hCritSec = pDevExtIn->hCritSecAppList; break;}
        default:
        {
        char sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

            // set TraceDetail
            RtlStringCchPrintfA(sTraceDetail,
                                _countof(sTraceDetail),
                                "PnDev[%s]: ########## Invalid CriticalSection!",
                                pDevExtIn->sPciLocShortAscii);

            // set trace
            fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                        ePNDEV_TRACE_CONTEXT_BASIC,
                        TRUE,                       // logging
                        sTraceDetail);

            // set FatalAsync
            pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__ERROR_CRIT_SEC;

            break;
        }
    }

    if  (hCritSec != NULL)
        // success
    {
        // enter CriticalSection
        WdfWaitLockAcquire(hCritSec, NULL);
    }
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
void fnEnterCritSec(DEVICE_EXTENSION*   pDevExtIn,
                    ePNDEV_CRIT_SEC_SYS eCritSecIn)
{
HANDLE  hCritSec = NULL;
char    sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

    switch  (eCritSecIn)
            // CritSec
    {
        case ePNDEV_CRIT_SEC_SYS_IOCTL:     {hCritSec = &pDevExtIn->uCritSecIoctl;      break;}
        case ePNDEV_CRIT_SEC_SYS_PAGE:      {hCritSec = &pDevExtIn->uCritSecPage;       break;}
        case ePNDEV_CRIT_SEC_SYS_APP_LIST:  {hCritSec = &pDevExtIn->uCritSecAppList;    break;}
        default:
        {
            // set TraceDetail
            RtlStringCchPrintfA(sTraceDetail,
                                _countof(sTraceDetail),
                                "PnDev[%s]: ########## Invalid CriticalSection!",
                                pDevExtIn->sPciLocShortAscii);

            // set trace
            fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                        ePNDEV_TRACE_CONTEXT_BASIC,
                        TRUE,                       // logging
                        sTraceDetail);

            // set FatalAsync
            pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__ERROR_CRIT_SEC;

            break;
        }
    }

    if  (hCritSec != NULL)
        // success
    {
        if  (pthread_mutex_lock(hCritSec) != ADN_OK)
            // error at entering CriticalSection
        {
            // set TraceDetail
            RtlStringCchPrintfA(sTraceDetail,
                                _countof(sTraceDetail),
                                "PnDev[%s]: ########## Error at pthread_mutex_lock()!",
                                pDevExtIn->sPciLocShortAscii);

            // set trace
            fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                        ePNDEV_TRACE_CONTEXT_BASIC,
                        TRUE,                       // logging
                        sTraceDetail);

            // set FatalAsync
            pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__ERROR_CRIT_SEC;
        }
    }
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
void fnEnterCritSec(DEVICE_EXTENSION*   pDevExtIn,
                    ePNDEV_CRIT_SEC_SYS eCritSecIn)
{
HANDLE  hCritSec = NULL;
UINT32  lSubClass = 0;
char    sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

    switch  (eCritSecIn)
            // CritSec
    {
        case ePNDEV_CRIT_SEC_SYS_IOCTL:     {hCritSec = &pDevExtIn->uCritSecIoctl;      lSubClass = 1;  break;}
        case ePNDEV_CRIT_SEC_SYS_PAGE:      {hCritSec = &pDevExtIn->uCritSecPage;       lSubClass = 2;  break;}
        case ePNDEV_CRIT_SEC_SYS_APP_LIST:  {hCritSec = &pDevExtIn->uCritSecAppList;    lSubClass = 2;  break;}
        default:
        {
            // set TraceDetail
            RtlStringCchPrintfA(sTraceDetail,
                                _countof(sTraceDetail),
                                "PnDev[%s]: ########## Invalid CriticalSection!",
                                pDevExtIn->sPciLocShortAscii);

            // set trace
            fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                        ePNDEV_TRACE_CONTEXT_BASIC,
                        TRUE,                       // logging
                        sTraceDetail);

            // set FatalAsync
            pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__ERROR_CRIT_SEC;

            break;
        }
    }

    if  (hCritSec != NULL)
        // success
    {
        // mutex_lock_nested is neccasary, if more than one mutex needs to be holded at one time.
        // IOCTL mutex protects ioctl on device.
        // PAGE and APP_LIST mutexes can be taken while executing an ioctl.
        // Therefore IOCTL, PAGE and APP_LIST mutexes have to be locked with mutex_lock_nested.
        mutex_lock_nested((struct mutex*) hCritSec, lSubClass);
        // mutex_lock_nested has no return value
    }
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  leave CriticalSection
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
void fnLeaveCritSec(DEVICE_EXTENSION*   pDevExtIn,
                    ePNDEV_CRIT_SEC_SYS eCritSecIn)
{
HANDLE hCritSec = NULL;

    switch  (eCritSecIn)
            // CritSec
    {
        case ePNDEV_CRIT_SEC_SYS_IOCTL:     {hCritSec = pDevExtIn->hCritSecIoctl;   break;}
        case ePNDEV_CRIT_SEC_SYS_PAGE:      {hCritSec = pDevExtIn->hCritSecPage;    break;}
        case ePNDEV_CRIT_SEC_SYS_APP_LIST:  {hCritSec = pDevExtIn->hCritSecAppList; break;}
        default:
        {
        char sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

            // set TraceDetail
            RtlStringCchPrintfA(sTraceDetail,
                                _countof(sTraceDetail),
                                "PnDev[%s]: ########## Invalid CriticalSection!",
                                pDevExtIn->sPciLocShortAscii);

            // set trace
            fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                        ePNDEV_TRACE_CONTEXT_BASIC,
                        TRUE,                       // logging
                        sTraceDetail);

            // set FatalAsync
            pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__ERROR_CRIT_SEC;

            break;
        }
    }

    if  (hCritSec != NULL)
        // success
    {
        // leave CriticalSection
        WdfWaitLockRelease(hCritSec);
    }
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
void fnLeaveCritSec(DEVICE_EXTENSION*   pDevExtIn,
                    ePNDEV_CRIT_SEC_SYS eCritSecIn)
{
HANDLE  hCritSec = NULL;
char    sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

    switch  (eCritSecIn)
            // CritSec
    {
        case ePNDEV_CRIT_SEC_SYS_IOCTL:     {hCritSec = &pDevExtIn->uCritSecIoctl;  break;}
        case ePNDEV_CRIT_SEC_SYS_PAGE:      {hCritSec = &pDevExtIn->uCritSecPage;   break;}
        case ePNDEV_CRIT_SEC_SYS_APP_LIST:  {hCritSec = &pDevExtIn->uCritSecAppList;    break;}
        default:
        {
            // set TraceDetail
            RtlStringCchPrintfA(sTraceDetail,
                                _countof(sTraceDetail),
                                "PnDev[%s]: ########## Invalid CriticalSection!",
                                pDevExtIn->sPciLocShortAscii);

            // set trace
            fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                        ePNDEV_TRACE_CONTEXT_BASIC,
                        TRUE,                       // logging
                        sTraceDetail);

            // set FatalAsync
            pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__ERROR_CRIT_SEC;

            break;
        }
    }

    if  (hCritSec != NULL)
        // success
    {
        if  (pthread_mutex_unlock(hCritSec) != ADN_OK)
            // error at leaving CriticalSection
        {
            // set TraceDetail
            RtlStringCchPrintfA(sTraceDetail,
                                _countof(sTraceDetail),
                                "PnDev[%s]: ########## Error at pthread_mutex_unlock()!",
                                pDevExtIn->sPciLocShortAscii);

            // set trace
            fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                        ePNDEV_TRACE_CONTEXT_BASIC,
                        TRUE,                       // logging
                        sTraceDetail);

            // set FatalAsync
            pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__ERROR_CRIT_SEC;
        }
    }
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
void fnLeaveCritSec(DEVICE_EXTENSION*   pDevExtIn,
                    ePNDEV_CRIT_SEC_SYS eCritSecIn)
{
HANDLE  hCritSec = NULL;
char    sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

    switch  (eCritSecIn)
            // CritSec
    {
        case ePNDEV_CRIT_SEC_SYS_IOCTL:     {hCritSec = &pDevExtIn->uCritSecIoctl;  break;}
        case ePNDEV_CRIT_SEC_SYS_PAGE:      {hCritSec = &pDevExtIn->uCritSecPage;   break;}
        case ePNDEV_CRIT_SEC_SYS_APP_LIST:  {hCritSec = &pDevExtIn->uCritSecAppList;    break;}
        default:
        {
            // set TraceDetail
            RtlStringCchPrintfA(sTraceDetail,
                                _countof(sTraceDetail),
                                "PnDev[%s]: ########## Invalid CriticalSection!",
                                pDevExtIn->sPciLocShortAscii);

            // set trace
            fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                        ePNDEV_TRACE_CONTEXT_BASIC,
                        TRUE,                       // logging
                        sTraceDetail);

            // set FatalAsync
            pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__ERROR_CRIT_SEC;

            break;
        }
    }

    if  (hCritSec != NULL)
        // success
    {
        // Unlock critical section
        mutex_unlock((struct mutex*) hCritSec);
        // mutex_unlock has no return value
    }
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  start up resources of periodic SysThread
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN fnStartUpThreadResSys(  DEVICE_EXTENSION*   pDevExtIn,
                                const UINT32        lPrioThreadIn,
                                const UINT32        lStackSizeThreadIn,
                                const _TCHAR*       sNameThreadIn,
                                const UINT32        lDimErrorIn,
                                UINT32*             pThreadIdOut,
                                _TCHAR*             sErrorOut)
{
BOOLEAN bResult         = FALSE;
BOOLEAN bResultThread   = FALSE;
_TCHAR  sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

    UNREFERENCED_PARAMETER(lStackSizeThreadIn);
    UNREFERENCED_PARAMETER(sNameThreadIn);

    // preset OutputParameter
    *pThreadIdOut = 0;

    // initialize Events:
    //  - SynchronisationEvent = AutoResetEvent
    //  - when event is set a single waiting thread becomes eligible for execution
    //  - the kernel automatically resets the event to the not-signaled state each time a wait is satisfied
    //  - initial state of the event = not-signaled
    KeInitializeEvent(  &pDevExtIn->uEventThreadStop,
                        SynchronizationEvent,
                        FALSE);

    // create SysThread
    {
    NTSTATUS    lResultTmp  = STATUS_UNSUCCESSFUL;
    HANDLE      hThreadTmp  = NULL;

        lResultTmp = PsCreateSystemThread(  &hThreadTmp,
                                            THREAD_ALL_ACCESS,
                                            NULL,
                                            NULL,
                                            NULL,
                                            fnThreadSys,            // function
                                            (void*) pDevExtIn);     // FunctionArg

        if  (lResultTmp != STATUS_SUCCESS)
            // error
        {
            // set ErrorDetail
            _RtlStringCchPrintf(sErrorDetail,
                                _countof(sErrorDetail),
                                _TEXT("(): Error at PsCreateSystemThread() (status=%!STATUS!)!"),
                                lResultTmp);

            // set ErrorString
            fnBuildString(  _TEXT(__FUNCTION__),
                            sErrorDetail,
                            NULL,
                            lDimErrorIn,
                            sErrorOut);
        }
        else
        {
            // create ThreadObject
            //  - then another thread can wait for completion of SysThread (ThreadObjects are themselves a type of DispatcherObject)
            //  - convert ThreadHandle into a pointer to the ThreadObject, then close ThreadHandle
            lResultTmp = ObReferenceObjectByHandle( hThreadTmp,
                                                    THREAD_ALL_ACCESS,
                                                    NULL,
                                                    KernelMode,
                                                    &pDevExtIn->hThreadSys,
                                                    NULL );

            if  (lResultTmp != STATUS_SUCCESS)
                // error
            {
                // set ErrorDetail
                _RtlStringCchPrintf(sErrorDetail,
                                    _countof(sErrorDetail),
                                    _TEXT("(): Error at ObReferenceObjectByHandle() (status=%!STATUS!)!"),
                                    lResultTmp);

                // set ErrorString
                fnBuildString(  _TEXT(__FUNCTION__),
                                sErrorDetail,
                                NULL,
                                lDimErrorIn,
                                sErrorOut);
            }
            else
            {
                // success
                bResultThread = TRUE;

                // set ThreadPriority
                {
                    //#define LOW_PRIORITY          0       // lowest thread priority level
                    //#define LOW_REALTIME_PRIORITY 16      // lowest realtime priority level
                    //#define HIGH_PRIORITY         31      // highest thread priority level
                    //#define MAXIMUM_PRIORITY      32      // number of thread priority levels

                    KeSetPriorityThread(pDevExtIn->hThreadSys,
                                        (KPRIORITY) lPrioThreadIn);
                }

                // driver must close ThreadHandle with ZwClose once the handle is no longer in use
                ZwClose(hThreadTmp);

                // return
                *pThreadIdOut = (UINT32) pDevExtIn->hThreadSys;
            }
        }
    }

    if  (bResultThread)
        // success
    {
        // success
        bResult = TRUE;
/*
    NTSTATUS                lResultTimer = STATUS_UNSUCCESSFUL;
    WDFTIMER                hTimer;
    WDF_TIMER_CONFIG        uTimerConfig;
    WDF_OBJECT_ATTRIBUTES   uAttributes;

        WDF_TIMER_CONFIG_INIT_PERIODIC( &uTimerConfig,
                                        fnEvtTimer,
                                        100);           // interval in msec when EvtTimerCbf is called repeatedly

        // use own SpinLock for more flexibility
        uTimerConfig.AutomaticSerialization = FALSE;

        WDF_OBJECT_ATTRIBUTES_INIT(&uAttributes);
        uAttributes.ParentObject = pDevExtIn->hDeviceOs;

        lResultTimer = WdfTimerCreate(  &uTimerConfig,
                                        &uAttributes,
                                        &pDevExtIn->hTimer);

        if  (lResultTimer != STATUS_SUCCESS)
            // error
        {
            // set ErrorDetail
            _RtlStringCchPrintf(sErrorDetail,
                                _countof(sErrorDetail),
                                _TEXT("(): Error at WdfTimerCreate() (status=%!STATUS!)!"),
                                lResultTimer);

            // set ErrorString
            fnBuildString(  _TEXT(__FUNCTION__),
                            sErrorDetail,
                            NULL,
                            lDimErrorIn,
                            sErrorOut);
        }
        else
        {
        BOOLEAN bTimerInQueue = FALSE;

            // start PeriodicTimer
            //  - first call of EvtTimerCbf when parameter DueTime elapses (negative DueTime -> relative to current system time)
            //  - afterwards interval specified at calling of WdfTimerCreate is relevant
            //  - returns TRUE if the timer object was in the system's timer queue (time period has not elapsed or periodic timer)
            bTimerInQueue = WdfTimerStart(  pDevExtIn->hTimer,
                                            WDF_REL_TIMEOUT_IN_MS(100));    // DueTime in msec

            // success
            bResult = TRUE;
        }
*/
    }

    return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
BOOLEAN fnStartUpThreadResSys(  DEVICE_EXTENSION*   pDevExtIn,
                                const UINT32        lPrioThreadIn,
                                const UINT32        lStackSizeThreadIn,
                                const _TCHAR*       sNameThreadIn,
                                const UINT32        lDimErrorIn,
                                UINT32*             pThreadIdOut,
                                _TCHAR*             sErrorOut)
{
BOOLEAN bResult = FALSE;

    // preset OutputParameter
    *pThreadIdOut = 0;

    // create SysThread
    {
    pthread_attr_t uThreadPar;

        if  (fnSetThreadPar(lPrioThreadIn,
                            lStackSizeThreadIn,
                            sNameThreadIn,
                            lDimErrorIn,
                            &uThreadPar,
                            sErrorOut))
            // setting ThreadPar ok
        {
        int32_t         lResultTmp  = ADN_ERROR;
        timer_t         lTimerId    = 0;
        struct sigevent uSigEvent;

            // set SigEvent
            {
                uSigEvent.sigev_notify_function     =  fnThreadSys;         // function
                uSigEvent.sigev_value.sival_ptr     =  (void*) pDevExtIn;   // FunctionArg
                uSigEvent.sigev_notify_attributes   =  &uThreadPar;         // ThreadPar
                uSigEvent.sigev_notify              =  SIGEV_THREAD;        // create a cyclic thread, not a TimerCbf called by TimerInt
            }

            // create timer
            //  - TimerThread is started by timer_settime()
            lResultTmp = timer_create(  CLOCK_MONOTONIC,
                                        &uSigEvent,
                                        &lTimerId);

            if  (lResultTmp != ADN_OK)
                // error
            {
                // set ErrorString
                fnBuildString(  _TEXT(__FUNCTION__),
                                _TEXT("(): Error at timer_create()!"),
                                NULL,
                                lDimErrorIn,
                                sErrorOut);
            }
            else
            {
            UINT32              lTimeCycle_Sec = 0;
            UINT32              lTimeCycle_Nsec = 0;
            struct itimerspec   uTime;

                // set CycleTime in sec/nsec
                lTimeCycle_Sec  = (pDevExtIn->lTimeCycleThreadSys_10msec / 100);
                lTimeCycle_Nsec = (pDevExtIn->lTimeCycleThreadSys_10msec % 100) * (10*1000*1000);   // *10 -> msec,...

                // set OffsetTime
                //  - first start of TimerThread after this time
                //  - 0:         timer disabled
                //  - CycleTime: timer will start now
                //      (to get a periodic timer with no certain start point, it_value should be set to the same value as it_interval)
                uTime.it_value.tv_sec   =  lTimeCycle_Sec;
                uTime.it_value.tv_nsec  =  lTimeCycle_Nsec;

                // set CycleTime
                uTime.it_interval.tv_sec    =  lTimeCycle_Sec;
                uTime.it_interval.tv_nsec   =  lTimeCycle_Nsec;

                // start timer
                lResultTmp = timer_settime( lTimerId,
                                            0,
                                            &uTime,
                                            NULL);

                if  (lResultTmp != ADN_OK)
                    // error
                {
                    // set ErrorString
                    fnBuildString(  _TEXT(__FUNCTION__),
                                    _TEXT("(): Error at timer_settime()!"),
                                    NULL,
                                    lDimErrorIn,
                                    sErrorOut);
                }
                else
                {
                struct sigevent uSigEvent;

                    // Note:
                    //  - ThreadId is stored at structure SigEvent

                    if (timer_getevent( lTimerId,
                                        &uSigEvent) != ADN_OK)
                        // error at getting SigEvent
                    {
                        // set ErrorString
                        fnBuildString(  _TEXT(__FUNCTION__),
                                        _TEXT("(): Error at timer_getevent()!"),
                                        NULL,
                                        lDimErrorIn,
                                        sErrorOut);
                    }
                    else
                    {
                        // success
                        bResult = TRUE;

                        // set ThreadHandle
                        //  - use TimerId of Adonis because it is required for timer_delete()
                        pDevExtIn->hThreadSys = (HANDLE) lTimerId;

                        // return
                        *pThreadIdOut = uSigEvent.sigev_thread;
                    }
                }
            }
        }
    }

    return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN fnStartUpThreadResSys(  DEVICE_EXTENSION*   pDevExtIn,
                                const UINT32        lPrioThreadIn,
                                const UINT32        lStackSizeThreadIn,
                                const _TCHAR*       sNameThreadIn,
                                const UINT32        lDimErrorIn,
                                UINT32*             pThreadIdOut,
                                _TCHAR*             sErrorOut)
{
BOOLEAN bResult = FALSE;
UINT32  lTimeCycle_Sec = 0;
UINT32  lTimeCycle_Msec = 0;

    //init timer with monotonic clock, so resetting the systemclock doesn't matter
    init_timer(&pDevExtIn->uSysThreadTimer);

    /* register the timer function an parameters*/
    pDevExtIn->uSysThreadTimer.function = &fnThreadSys;             // function called when timer expires
    pDevExtIn->uSysThreadTimer.data = (unsigned long) pDevExtIn;    // parameter to fnThreadSys

    //calculate CycleTime in sec/nsec
    lTimeCycle_Sec  = (pDevExtIn->lTimeCycleThreadSys_10msec / 100);
    lTimeCycle_Msec = (pDevExtIn->lTimeCycleThreadSys_10msec % 100) * 10; //MSec = 10msec * 10 => 10ms = 1 * 10ms

    pDevExtIn->uSysThreadTimer.expires = jiffies +                        //current ticks
                                         (HZ * lTimeCycle_Sec) +          //secounds (HZ are ticks per second)
                                         (HZ * lTimeCycle_Msec)/1000;     //msecs (HZ/1000 are ticks per millisecond)

    // start the timer
    add_timer(&pDevExtIn->uSysThreadTimer);

    // no ThreadId in Linux
    *pThreadIdOut = 0xFF;

    bResult = TRUE;

    return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  shut down resources of periodic SysThread
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN fnShutDownThreadResSys( DEVICE_EXTENSION*   pDevExtIn,
                                const UINT32        lDimErrorIn,
                                _TCHAR*             sErrorOut)
{
BOOLEAN bResult = FALSE;

    if  (pDevExtIn->hThreadSys == NULL)
        // SysThread not running
    {
        // success
        bResult = TRUE;
    }
    else
    {
    NTSTATUS        lResultEvent = STATUS_UNSUCCESSFUL;
    LARGE_INTEGER   uTimeOut;

        // Timeout: 30.000.000 * 100nsec = 3sec (negative: relative time)
        uTimeOut.QuadPart = -30000000;

        // set EventThreadStop
        //  - call of KeSetEvent() requires IRQL <= DISPATCH_LEVEL!
        KeSetEvent( &pDevExtIn->uEventThreadStop,
                    IO_NO_INCREMENT,
                    FALSE);

        // wait for stop of SysThread
        //  - Windows: possible because of ObReferenceObjectByHandle
        lResultEvent = KeWaitForSingleObject(   pDevExtIn->hThreadSys,
                                                Executive,
                                                KernelMode,
                                                FALSE,
                                                &uTimeOut );

        if  (lResultEvent != STATUS_SUCCESS)
            // error
        {
            // set ErrorString
            fnBuildString(  _TEXT(__FUNCTION__),
                            _TEXT("(): Error at waiting for event!"),
                            NULL,
                            lDimErrorIn,
                            sErrorOut);
        }
        else
        {
            // success
            bResult = TRUE;
        }

        ObDereferenceObject(pDevExtIn->hThreadSys);
        pDevExtIn->hThreadSys = NULL;
    }
/*
    // stop PeriodicTimer
    {
    BOOLEAN bTimerInQueue = FALSE;

        // stop PeriodicTimer
        //  - returns TRUE if the timer object was in the system timer queue (time period has not elapsed or periodic timer)
        //  - TRUE: framework does not return until all queued calls to driver's DPCs, including the driver's EvtTimerCbfs, have executed
        bTimerInQueue = WdfTimerStop(   pDevExtIn->hTimer,
                                        TRUE);
    }
*/
    return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
BOOLEAN fnShutDownThreadResSys( DEVICE_EXTENSION*   pDevExtIn,
                                const UINT32        lDimErrorIn,
                                _TCHAR*             sErrorOut)
{
BOOLEAN bResult = FALSE;

    if  (pDevExtIn->hThreadSys == NULL)
        // SysThread not running
    {
        // success
        bResult = TRUE;
    }
    else
    {
        if  (timer_delete((timer_t) pDevExtIn->hThreadSys) != ADN_OK)
            // error at deleting timer
        {
            // set ErrorString
            fnBuildString(  _TEXT(__FUNCTION__),
                            _TEXT("(): Error at timer_delete()!"),
                            NULL,
                            lDimErrorIn,
                            sErrorOut);
        }
        else
        {
            // success
            bResult = TRUE;

            // reset ThreadHandle
            pDevExtIn->hThreadSys = NULL;
        }
    }

    return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN fnShutDownThreadResSys( DEVICE_EXTENSION*   pDevExtIn,
                                const UINT32        lDimErrorIn,
                                _TCHAR*             sErrorOut)
{
BOOLEAN bResult = FALSE;

	if  (!timer_pending(&pDevExtIn->uSysThreadTimer))
		// timer not running
	{
		// success
		bResult = TRUE;
	}
	else
	{
		// set Flag: don't restart periodically timer
		pDevExtIn->bSysThreadTimerStop = TRUE;

		if  (del_timer_sync(&pDevExtIn->uSysThreadTimer) < LINUX_OK)
			// error at deleting timer
		{
			// set ErrorString
			fnBuildString(  _TEXT(__FUNCTION__),
							_TEXT("(): Error at del_timer_sync()!"),
							NULL,
							lDimErrorIn,
							sErrorOut);
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
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  start up resources of DllThread
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN fnStartUpThreadResDll(  DEVICE_EXTENSION*       pDevExtIn,
                                uPNDEV_APP*             pAppIn,
                                uPNDEV_THREAD_IOCTL*    pThreadIoctlIn,
                                const UINT32            lDimErrorIn,
                                _TCHAR*                 sErrorOut)
{
BOOLEAN     bResult     = FALSE;
NTSTATUS    lResult1    = STATUS_UNSUCCESSFUL;
NTSTATUS    lResult2    = STATUS_UNSUCCESSFUL;
_TCHAR      sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

    // validate access to EventXx, return corresponding ptr
    //  - increment RefCtr -> prevents the object from being deleted while the ptr is being referenced
    //  - must be in the context of the process that created the handle
    //  - AccessMode = KernelMode (pIrp->RequestorMode provides UserMode, access not ensured everywhere))

    // get KernelModeHandle
    lResult1 = ObReferenceObjectByHandle(   (HANDLE) pThreadIoctlIn->uEventUinfo.hHandle,
                                            SYNCHRONIZE,
                                            *ExEventObjectType,
                                            KernelMode,
                                            &pAppIn->hEventUinfo,
                                            NULL);

    // get KernelModeHandle
    lResult2 = ObReferenceObjectByHandle(   (HANDLE) pThreadIoctlIn->uEventUisr.hHandle,
                                            SYNCHRONIZE,
                                            *ExEventObjectType,
                                            KernelMode,
                                            &pAppIn->hEventUisr,
                                            NULL);

    if  (   (lResult1 != STATUS_SUCCESS)
        ||  (lResult2 != STATUS_SUCCESS))
        // error
    {
        // set ErrorDetail
        _RtlStringCchPrintf(sErrorDetail,
                            _countof(sErrorDetail),
                            _TEXT("(): Error at ObReferenceObjectByHandle() (status1=0x%x, status2=0x%x)"),
                            lResult1,
                            lResult2);

        // set ErrorString
        fnBuildString(  _TEXT(__FUNCTION__),
                        sErrorDetail,
                        NULL,
                        lDimErrorIn,
                        sErrorOut);
    }
    else if (   (pAppIn->hEventUinfo    == NULL)
            ||  (pAppIn->hEventUisr     == NULL))
        // error
    {
        // set ErrorString
        fnBuildString(  _TEXT(__FUNCTION__),
                        _TEXT("(): Invalid handle!"),
                        NULL,
                        lDimErrorIn,
                        sErrorOut);
    }
    else
    {
        // set UserModeHandle
        pAppIn->hEventUinfoUm   = (HANDLE) pThreadIoctlIn->uEventUinfoUm.hHandle;
        pAppIn->hThreadDll      = (HANDLE) pThreadIoctlIn->uThreadDll.hHandle;

        // success
        bResult = TRUE;
    }

    return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
BOOLEAN fnStartUpThreadResDll(  DEVICE_EXTENSION*       pDevExtIn,
                                uPNDEV_APP*             pAppIn,
                                uPNDEV_THREAD_IOCTL*    pThreadIoctlIn,
                                const UINT32            lDimErrorIn,
                                _TCHAR*                 sErrorOut)
{
BOOLEAN bResult = FALSE;

    // Note:
    //  - pThreadIoctlIn->hEventXx is an index for EventHandleTable
    //      -> may be 0 (see fnGetFreeIndexEventHandleTable())

    // set KernelModeHandle
    pAppIn->hEventUinfo = g_uVarSys.uEventHandleTable[(UINT32) pThreadIoctlIn->uEventUinfo.hHandle].pEventObject;
    pAppIn->hEventUisr  = g_uVarSys.uEventHandleTable[(UINT32) pThreadIoctlIn->uEventUisr.hHandle].pEventObject;

    // set UserModeHandle
    pAppIn->hEventUinfoUm   = (HANDLE) pThreadIoctlIn->uEventUinfoUm.hHandle;
    pAppIn->hThreadDll      = (HANDLE) pThreadIoctlIn->uThreadDll.hHandle;

    if  (   (pAppIn->hEventUinfo    == NULL)
        ||  (pAppIn->hEventUinfoUm  == NULL)
        ||  (pAppIn->hEventUisr     == NULL)
        ||  (pAppIn->hThreadDll     == NULL))
        // error
    {
        // set ErrorString
        fnBuildString(  _TEXT(__FUNCTION__),
                        _TEXT("(): Invalid handle!"),
                        NULL,
                        lDimErrorIn,
                        sErrorOut);
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
BOOLEAN fnGetEventFdFile(   DEVICE_EXTENSION*   pDevExtIn,
                            INT32               lEventfdIn,
                            void**              pEventfileOut,
                            const UINT32        lDimErrorIn,
                            _TCHAR*             sErrorOut)
{
BOOLEAN             bResult         = FALSE;
struct file*        pEventfdFile    = NULL;     // eventfd's file struct
char sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

    // preset OutputParameter
    *pEventfileOut = NULL;

    if(lEventfdIn != -1)
    {
		// fcheck_files has to be protected by rcu or file lock!
		rcu_read_lock();
		// Resolve pointer to the userspace program's eventfd's file struct
		pEventfdFile = fcheck_files(    current->files,
										lEventfdIn);
		rcu_read_unlock();

		if(pEventfdFile != NULL)
		{
			// Return resolved pointer to the userspace program's eventfd's file struct
			*pEventfileOut = pEventfdFile;

			// success
			bResult = TRUE;
		}
		else
		{
			// set TraceDetail
			RtlStringCchPrintfA(sTraceDetail,
								_countof(sTraceDetail),
								"PnDev[%s]: ########## Can't find userspace eventfd file to send event!",
								pDevExtIn->sPciLocShortAscii);

			// set trace
			fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
						ePNDEV_TRACE_CONTEXT_BASIC,
						TRUE,                       // logging
						sTraceDetail);

			// set FatalAsync
			pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__ERROR_EVENT;
		}
    }

    return(bResult);
}


//------------------------------------------------------------------------
BOOLEAN fnStartUpThreadResDll(  DEVICE_EXTENSION*       pDevExtIn,
                                uPNDEV_APP*             pAppIn,
                                uPNDEV_THREAD_IOCTL*    pThreadIoctlIn,
                                const UINT32            lDimErrorIn,
                                _TCHAR*                 sErrorOut)
{
BOOLEAN bResult = FALSE;

    // Note:
    //  - pThreadIoctlIn->hEventXx is eventfd from usermode program
    //      -> it has to be resolved it's context send events from kernel mode
    //      -> We have to save the addresses to the eventfd files, while executing an ioctl
    //          since it has to be done in context of the application to get the files which belong to the application.
    //      -> The eventfd context has to be resolved right before sending an event,
    //          this will be done with eventfd_ctx_fileget and the saved file address.


    // Get eventfd file adress out of eventfd descriptor and current->pid
    fnGetEventFdFile(   pDevExtIn,
                        pThreadIoctlIn->uEventUinfo.lUint64,
                        &pAppIn->hEventUinfo,
                        lDimErrorIn,
                        sErrorOut);
    fnGetEventFdFile(   pDevExtIn,
                        pThreadIoctlIn->uEventUisr.lUint64,
                        &pAppIn->hEventUisr,
                        lDimErrorIn,
                        sErrorOut);

    if  (   (   pAppIn->hEventUinfo     == NULL)
        ||  (   pAppIn->hEventUisr      == NULL))
        // error
    {
        // set ErrorString
        fnBuildString(  _TEXT(__FUNCTION__),
                        _TEXT("(): Invalid handle!"),
                        NULL,
                        lDimErrorIn,
                        sErrorOut);
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

//************************************************************************
//  D e s c r i p t i o n :
//
//  shut down resources of DllThread
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN fnShutDownThreadResDll( DEVICE_EXTENSION*   pDevExtIn,
                                uPNDEV_APP*         pAppIn,
                                const UINT32        lDimErrorIn,
                                _TCHAR*             sErrorOut)
{
BOOLEAN bResult = FALSE;

    // success
    bResult = TRUE;

    // decrement RefCtr of events
    {
        if  (pAppIn->hEventUinfo != NULL)
            // valid event
        {
            ObDereferenceObject(pAppIn->hEventUinfo);
            pAppIn->hEventUinfo = NULL;
        }

        if  (pAppIn->hEventUisr != NULL)
            // valid event
        {
            ObDereferenceObject(pAppIn->hEventUisr);
            pAppIn->hEventUisr = NULL;
        }
    }

    pAppIn->hEventUinfoUm   = NULL;
    pAppIn->hThreadDll      = NULL;

    return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
BOOLEAN fnShutDownThreadResDll( DEVICE_EXTENSION*   pDevExtIn,
                                uPNDEV_APP*         pAppIn,
                                const UINT32        lDimErrorIn,
                                _TCHAR*             sErrorOut)
{
BOOLEAN bResult = FALSE;

    // success
    bResult = TRUE;

    pAppIn->hEventUinfo     = NULL;
    pAppIn->hEventUinfoUm   = NULL;
    pAppIn->hEventUisr      = NULL;
    pAppIn->hThreadDll      = NULL;

    return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN fnShutDownThreadResDll( DEVICE_EXTENSION*   pDevExtIn,
                                uPNDEV_APP*         pAppIn,
                                const UINT32        lDimErrorIn,
                                _TCHAR*             sErrorOut)
{
BOOLEAN bResult = FALSE;

    // success
    bResult = TRUE;

    pAppIn->hEventUinfo     = NULL;
    pAppIn->hEventUisr      = NULL;

    return(bResult);
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  set event of DllThread
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
void fnSetEventThreadDll(   DEVICE_EXTENSION*       pDevExtIn,
                            uPNDEV_APP*             pAppIn,
                            ePNDEV_EVENT_THREAD_DLL eEventIn,
                            BOOLEAN                 bThreadSysIn)
{
HANDLE hEvent = NULL;

    UNREFERENCED_PARAMETER(bThreadSysIn);

    switch  (eEventIn)
            // event
    {
        case ePNDEV_EVENT_THREAD_DLL_UINFO: {hEvent = pAppIn->hEventUinfo;  break;}
        case ePNDEV_EVENT_THREAD_DLL_UISR:  {hEvent = pAppIn->hEventUisr;   break;}
        default:
        {
        char sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

            // set TraceDetail
            RtlStringCchPrintfA(sTraceDetail,
                                _countof(sTraceDetail),
                                "PnDev[%s]: ########## Invalid event for DllThread!",
                                pDevExtIn->sPciLocShortAscii);

            // set trace
            fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                        ePNDEV_TRACE_CONTEXT_BASIC,
                        TRUE,                       // logging
                        sTraceDetail);

            // set FatalAsync
            pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__ERROR_EVENT;

            break;
        }
    }

    if  (hEvent != NULL)
        // success
    {
        // set event
        //  - call of KeSetEvent() requires IRQL <= DISPATCH_LEVEL!
        KeSetEvent( (KEVENT*) hEvent,
                    IO_NO_INCREMENT,
                    FALSE);
    }
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
void fnSetEventThreadDll(   DEVICE_EXTENSION*       pDevExtIn,
                            uPNDEV_APP*             pAppIn,
                            ePNDEV_EVENT_THREAD_DLL eEventIn,
                            BOOLEAN                 bThreadSysIn)
{
char sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

    // Note:
    //  - an Adonis thread is always a UserModeThread
    //      -> adn_evt_fire_event() causes exception!

    if  (bThreadSysIn)
        // called by SysThread (UserMode!)
    {
        if  (eEventIn != ePNDEV_EVENT_THREAD_DLL_UINFO)
            // not EventUinfo
        {
            // set TraceDetail
            RtlStringCchPrintfA(sTraceDetail,
                                _countof(sTraceDetail),
                                "PnDev[%s]: ########## Invalid event for DllThread!",
                                pDevExtIn->sPciLocShortAscii);

            // set trace
            fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                        ePNDEV_TRACE_CONTEXT_BASIC,
                        TRUE,                       // logging
                        sTraceDetail);

            // set FatalAsync
            pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__ERROR_EVENT;
        }
        else
        {
        int32_t lResultTmp = ADN_ERROR;

            // set event
            lResultTmp = pthread_kill(  (pthread_t) pAppIn->hThreadDll,
                                        (UINT32)    pAppIn->hEventUinfoUm);

            if  (lResultTmp != ADN_OK)
                // error
            {
                // set TraceDetail
                RtlStringCchPrintfA(sTraceDetail,
                                    _countof(sTraceDetail),
                                    "PnDev[%s]: ########## Error at pthread_kill()!",
                                    pDevExtIn->sPciLocShortAscii);

                // set trace
                fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                            ePNDEV_TRACE_CONTEXT_BASIC,
                            TRUE,                       // logging
                            sTraceDetail);

                // set FatalAsync
                pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__ERROR_EVENT;
            }
        }
    }
    else
    {
    HANDLE hEvent = NULL;

        switch  (eEventIn)
                // event
        {
            case ePNDEV_EVENT_THREAD_DLL_UINFO: {hEvent = pAppIn->hEventUinfo;  break;}
            case ePNDEV_EVENT_THREAD_DLL_UISR:  {hEvent = pAppIn->hEventUisr;   break;}
            default:
            {
                // set TraceDetail
                RtlStringCchPrintfA(sTraceDetail,
                                    _countof(sTraceDetail),
                                    "PnDev[%s]: ########## Invalid event for DllThread!",
                                    pDevExtIn->sPciLocShortAscii);

                // set trace
                fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                            ePNDEV_TRACE_CONTEXT_BASIC,
                            TRUE,                       // logging
                            sTraceDetail);

                // set FatalAsync
                pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__ERROR_EVENT;

                break;
            }
        }

        if  (hEvent != NULL)
            // success
        {
            // set event
            adn_evt_fire_event((adn_evt_event_t*) hEvent);
        }
    }
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
void fnSetEventThreadDll(   DEVICE_EXTENSION*       pDevExtIn,
                            uPNDEV_APP*             pAppIn,
                            ePNDEV_EVENT_THREAD_DLL eEventIn,
                            BOOLEAN                 bThreadSysIn)
{
char sTraceDetail[PNDEV_SIZE_STRING_TRACE]  = {0};
struct file*        pEventFile      = NULL;     // pointer to usermode eventfd file
struct eventfd_ctx* pEventfdCtx     = NULL;     // eventfd context

    switch  (eEventIn)
            // event
    {
        case ePNDEV_EVENT_THREAD_DLL_UINFO: {pEventFile = (struct file*) pAppIn->hEventUinfo;   break;}
        case ePNDEV_EVENT_THREAD_DLL_UISR:  {pEventFile = (struct file*) pAppIn->hEventUisr;    break;}
        default:
        {
            // set TraceDetail
            RtlStringCchPrintfA(sTraceDetail,
                                _countof(sTraceDetail),
                                "PnDev[%s]: ########## Invalid event for DllThread!",
                                pDevExtIn->sPciLocShortAscii);

            // set trace
            fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                        ePNDEV_TRACE_CONTEXT_BASIC,
                        TRUE,                       // logging
                        sTraceDetail);

            // set FatalAsync
            pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__ERROR_EVENT;

            break;
        }
    }

    if(pEventFile != NULL)
    {
        pEventfdCtx = eventfd_ctx_fileget(pEventFile);

        if  (pEventfdCtx != NULL)
            // Successfully resolved eventfd context
        {
            // Increment userspace program's eventfd's counter by eEventIn
            eventfd_signal( pEventfdCtx,
                            eEventIn);
            eventfd_ctx_put(pEventfdCtx);
        }
        else
        {
            // set TraceDetail
            RtlStringCchPrintfA(sTraceDetail,
                                _countof(sTraceDetail),
                                "PnDev[%s]: ########## Error at eventfd_ctx_fileget()!",
                                pDevExtIn->sPciLocShortAscii);

            // set trace
            fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                        ePNDEV_TRACE_CONTEXT_BASIC,
                        TRUE,                       // logging
                        sTraceDetail);

            // set FatalAsync
            pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__ERROR_EVENT;
        }
    }
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  get IoctlBuffer
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
ePNDEV_IOCTL fnGetIoctlBuf( HANDLE              hOsParIn,
                            const UINT32        lSizeInputBufIn,
                            const UINT32        lSizeOutputBufIn,
                            void**              pPtrInputBufOut,
                            void**              pPtrOutputBufOut)
{
ePNDEV_IOCTL    eResult = ePNDEV_IOCTL_INVALID;
NTSTATUS        lResult = STATUS_UNSUCCESSFUL;
char            sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

    // get InputBuffer
    lResult = WdfRequestRetrieveInputBuffer((WDFREQUEST) hOsParIn,
                                            lSizeInputBufIn,        // minimum required size
                                            pPtrInputBufOut,
                                            NULL);

    if  (lResult != STATUS_SUCCESS)
        // error
    {
        eResult = ePNDEV_IOCTL_ERROR_BUF_IN;

        // set TraceDetail
        RtlStringCchPrintfA(sTraceDetail,
                            _countof(sTraceDetail),
                            "PnDev[x,x,x]: ########## Error at WdfRequestRetrieveInputBuffer() (status=%!STATUS!)",
                            lResult);

        // set trace
        fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                    ePNDEV_TRACE_CONTEXT_BASIC,
                    TRUE,                       // logging
                    sTraceDetail);
    }
    else
    {
        // preset
        lResult = STATUS_UNSUCCESSFUL;

        // get OutputBuffer
        lResult = WdfRequestRetrieveOutputBuffer(   (WDFREQUEST) hOsParIn,
                                                    lSizeOutputBufIn,       // minimum required size
                                                    pPtrOutputBufOut,
                                                    NULL);

        if  (lResult != STATUS_SUCCESS)
            // error
        {
            eResult = ePNDEV_IOCTL_ERROR_BUF_OUT;

            // set TraceDetail
            RtlStringCchPrintfA(sTraceDetail,
                                _countof(sTraceDetail),
                                "PnDev[x,x,x]: ########## Error at WdfRequestRetrieveOutputBuffer() (status=%!STATUS)!",
                                lResult);

            // set trace
            fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                        ePNDEV_TRACE_CONTEXT_BASIC,
                        TRUE,                       // logging
                        sTraceDetail);
        }
        else
        {
            // success
            eResult = ePNDEV_IOCTL_OK;
        }
    }

    return(eResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
ePNDEV_IOCTL fnGetIoctlBuf( HANDLE          hOsParIn,
                            const UINT32    lSizeInputBufIn,
                            const UINT32    lSizeOutputBufIn,
                            void**          pPtrInputBufOut,
                            void**          pPtrOutputBufOut)
{
ePNDEV_IOCTL eResult = ePNDEV_IOCTL_INVALID;

    // Note:
    //  - at Adonis this function isn't nessecary

    return(eResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
ePNDEV_IOCTL fnGetIoctlBuf( HANDLE          hOsParIn,
                            const UINT32    lSizeInputBufIn,
                            const UINT32    lSizeOutputBufIn,
                            void**          pPtrInputBufOut,
                            void**          pPtrOutputBufOut)
{
ePNDEV_IOCTL eResult = ePNDEV_IOCTL_INVALID;

    // Note:
    //  - at Linux this function isn't nessecary

    return(eResult);
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  IoctlHandler of service GetDeviceInfoSys
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN fnIoctlGetDeviceInfoSys(uPNDEV_GET_DEVICE_INFO_SYS_IN*  pIn,
                                uPNDEV_GET_DEVICE_INFO_SYS_OUT* pOut)
{
BOOLEAN bResult = FALSE;

    // Note:
    //  - at Windows this service will never be called by DLL

    // set ErrorString
    //  - not changed by DLL
    fnBuildString(  PNDEV_ERROR_SUPPORT,
                    NULL,
                    NULL,
                    _countof(pOut->sError),
                    pOut->sError);

    return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS) || defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN fnIoctlGetDeviceInfoSys(uPNDEV_GET_DEVICE_INFO_SYS_IN*  pIn,
                                uPNDEV_GET_DEVICE_INFO_SYS_OUT* pOut)
{
BOOLEAN bResult         = FALSE;
UINT32  lIndexDevice    = 0;

    // Note:
    //  - simulate METHOD_BUFFERED: memory of input buffer is used as output buffer!
    //  - OutputParameter must be preset!
    {
        // copy input data
        lIndexDevice = pIn->lIndexDevice;

        // preset OutputParameter
        RtlZeroMemory(  pOut,
                        sizeof(uPNDEV_GET_DEVICE_INFO_SYS_OUT));
    }

    if  (g_uVarSys.lCtrDevice == 0)
        // no device exist
    {
        // success
        bResult = TRUE;
    }
    else
    {
        if  (pIn->lIndexDevice > (g_uVarSys.lCtrDevice - 1))
            // invalid DeviceIndex
        {
        _TCHAR sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

            // set ErrorDetail
            _RtlStringCchPrintf(sErrorDetail,
                                _countof(sErrorDetail),
                                _TEXT("(): Invalid DeviceIndex (real: %u, max: %u)!"),
                                pIn->lIndexDevice,
                                (g_uVarSys.lCtrDevice - 1));

            // set ErrorString
            fnBuildString(  _TEXT(__FUNCTION__),
                            sErrorDetail,
                            NULL,
                            _countof(pOut->sError),
                            pOut->sError);
        }
        else
        {
        uLIST_HEADER*   pList       = NULL;
        uLIST_HEADER*   pBlockTmp   = NULL;
        UINT32          i = 0;

            // success
            bResult = TRUE;

            pList = &g_uVarSys.uListDevice;

            // get first block of DeviceList
            pBlockTmp = (uLIST_HEADER*) pList->pNext;

            while   (pBlockTmp != pList)
                    // end of list not reached
            {
                if  (i == pIn->lIndexDevice)
                    // desired DeviceIndex
                {
                DEVICE_EXTENSION* pDevExt = (DEVICE_EXTENSION*) pBlockTmp;

                    // return
                    {
                        pOut->lCtrDevice    = g_uVarSys.lCtrDevice;
                        pOut->eBoard        = pDevExt->eBoard;

                        _RtlStringCchCopy(  pOut->sPciLoc,
                                            _countof(pOut->sPciLoc),
                                            pDevExt->uPciLoc.sString);
                    }

                    // leave loop
                    break;
                }

                // get next block of DeviceList
                pBlockTmp = (uLIST_HEADER*) pBlockTmp->pNext;

                i++;
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
//  IoctlHandler of service GetDeviceHandleSys
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN fnIoctlGetDeviceHandleSys(  uPNDEV_GET_DEVICE_HANDLE_SYS_IN*    pIn,
                                    uPNDEV_GET_DEVICE_HANDLE_SYS_OUT*   pOut)
{
BOOLEAN bResult = FALSE;

    // Note:
    //  - at Windows this service will never be called by DLL

    // set ErrorString
    //  - not changed by DLL
    fnBuildString(  PNDEV_ERROR_SUPPORT,
                    NULL,
                    NULL,
                    _countof(pOut->sError),
                    pOut->sError);

    return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS) || defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN fnIoctlGetDeviceHandleSys(  uPNDEV_GET_DEVICE_HANDLE_SYS_IN*    pIn,
                                    uPNDEV_GET_DEVICE_HANDLE_SYS_OUT*   pOut)
{
BOOLEAN bResult = FALSE;
_TCHAR  sPciLoc[PNDEV_SIZE_STRING_BUF_SHORT] = {0};

    // Note:
    //  - simulate METHOD_BUFFERED: memory of input buffer is used as output buffer!
    //  - OutputParameter must be preset!
    {
        // copy input data
        _RtlStringCchCopy(  sPciLoc,
                            _countof(sPciLoc),
                            pIn->sPciLoc);

        // preset OutputParameter
        RtlZeroMemory(  pOut,
                        sizeof(uPNDEV_GET_DEVICE_HANDLE_SYS_OUT));
    }

    if  (g_uVarSys.lCtrDevice > 0)
        // device exist
    {
    uLIST_HEADER*   pList       = NULL;
    uLIST_HEADER*   pBlockTmp   = NULL;

        // success
        bResult = TRUE;

        pList = &g_uVarSys.uListDevice;

        // get first block of DeviceList
        pBlockTmp = (uLIST_HEADER*) pList->pNext;

        while   (pBlockTmp != pList)
                // end of list not reached
        {
        DEVICE_EXTENSION* pDevExt = (DEVICE_EXTENSION*) pBlockTmp;

            if  (_tcscmp(pDevExt->uPciLoc.sString, pIn->sPciLoc) == 0)
                // device with desired PciLoc found
            {
                // success
                bResult = TRUE;

                // return
                pOut->uDeviceSys.hHandle = pDevExt;

                // leave loop
                break;
            }

            // get next block of DeviceList
            pBlockTmp = (uLIST_HEADER*) pBlockTmp->pNext;
        }
    }

    if  (!bResult)
        // device not found
    {
        // set ErrorString
        //  - changed by DLL to boolean
        fnBuildString(  PNDEV_ERROR_DEVICE_NOT_FOUND,
                        NULL,
                        NULL,
                        _countof(pOut->sError),
                        pOut->sError);
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
//  IoctlHandler of service AllocEventId
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN fnIoctlAllocEventId(uPNDEV_ALLOC_EVENT_ID_IN*   pIn,
                            uPNDEV_ALLOC_EVENT_ID_OUT*  pOut)
{
BOOLEAN bResult = FALSE;

    // Note:
    //  - at Windows this service will never be called by DLL

    // set ErrorString
    //  - not changed by DLL
    fnBuildString(  PNDEV_ERROR_SUPPORT,
                    NULL,
                    NULL,
                    _countof(pOut->sError),
                    pOut->sError);

    return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
BOOLEAN fnIoctlAllocEventId(uPNDEV_ALLOC_EVENT_ID_IN*   pIn,
                            uPNDEV_ALLOC_EVENT_ID_OUT*  pOut)
{
BOOLEAN bResult = FALSE;

    // Note:
    //  - simulate METHOD_BUFFERED: memory of input buffer is used as output buffer!
    //  - OutputParameter must be preset!
    {
        // copy input data
        //  - no parameter

        // preset OutputParameter
        RtlZeroMemory(  pOut,
                        sizeof(uPNDEV_ALLOC_EVENT_ID_OUT));
    }

    if (fnGetFreeIndexEventHandleTable( PNDEV_DIM_ARRAY_EVENT_HANDLE_TABLE,
                                        g_uVarSys.uEventHandleTable,
                                        _countof(pOut->sError),
                                        &pOut->lEventId,
                                        pOut->sError))
        // getting first free index of EventHandleTable ok
    {
        // success
        bResult = TRUE;
    }

    return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN fnIoctlAllocEventId(uPNDEV_ALLOC_EVENT_ID_IN*   pIn,
                            uPNDEV_ALLOC_EVENT_ID_OUT*  pOut)
{
BOOLEAN bResult = FALSE;

    // Note:
    //  - at Linux this service will never be called by DLL

    // set ErrorString
    //  - not changed by DLL
    fnBuildString(  PNDEV_ERROR_SUPPORT,
                    NULL,
                    NULL,
                    _countof(pOut->sError),
                    pOut->sError);

    return(bResult);
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  IoctlHandler of service FreeEventId
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN fnIoctlFreeEventId( uPNDEV_FREE_EVENT_ID_IN*    pIn,
                            uPNDEV_FREE_EVENT_ID_OUT*   pOut)
{
BOOLEAN bResult = FALSE;

    // Note:
    //  - at Windows this service will never be called by DLL

    // set ErrorString
    //  - not changed by DLL
    fnBuildString(  PNDEV_ERROR_SUPPORT,
                    NULL,
                    NULL,
                    _countof(pOut->sError),
                    pOut->sError);

    return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
BOOLEAN fnIoctlFreeEventId( uPNDEV_FREE_EVENT_ID_IN*    pIn,
                            uPNDEV_FREE_EVENT_ID_OUT*   pOut)
{
BOOLEAN bResult     = FALSE;
UINT32  lEventId    = 0;

    // Note:
    //  - simulate METHOD_BUFFERED: memory of input buffer is used as output buffer!
    //  - OutputParameter must be preset!
    {
        // copy input data
        lEventId = pIn->lEventId;

        // preset OutputParameter
        RtlZeroMemory(  pOut,
                        sizeof(uPNDEV_FREE_EVENT_ID_OUT));
    }

    if (fnReleaseIndexEventHandleTable( lEventId,
                                        PNDEV_DIM_ARRAY_EVENT_HANDLE_TABLE,
                                        g_uVarSys.uEventHandleTable,
                                        _countof(pOut->sError),
                                        pOut->sError))
        // releasing used index of EventHandleTable ok
    {
        // success
        bResult = TRUE;
    }

    return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN fnIoctlFreeEventId( uPNDEV_FREE_EVENT_ID_IN*    pIn,
                            uPNDEV_FREE_EVENT_ID_OUT*   pOut)
{
BOOLEAN bResult     = FALSE;

    // ToDo

    return(bResult);
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  OS abstraction for NetProfi Kernel IOCTL handling
//************************************************************************
//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN fnIoctlDoNetProfi(  DEVICE_EXTENSION*           pDevExtIn,
                            uPNDEV_NP_DO_NETPROFI_REQ_IN*  pOnlParIn,
                            uPNDEV_NP_DO_NETPROFI_REQ_OUT* pOnlParOut)
{
BOOLEAN bResult = FALSE;

    // wrapper for NetProfi Kernel
    bResult = fnIoctlDoNetProfi2(   pDevExtIn,
                                    pOnlParIn,
                                    pOnlParOut);

    return (bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
BOOLEAN fnIoctlDoNetProfi(  DEVICE_EXTENSION*           pDevExtIn,
                            uPNDEV_NP_DO_NETPROFI_REQ_IN*  pOnlParIn,
                            uPNDEV_NP_DO_NETPROFI_REQ_OUT* pOnlParOut)
{
BOOLEAN bResult = FALSE;

    // Note:
    //  - at Adonis this service will never be called by DLL

    // set ErrorString
    //  - not changed by DLL
    fnBuildString(  _TEXT(__FUNCTION__),
                    _TEXT("(): OperatingSystem not supported."),
                    NULL,
                    _countof(pOnlParOut->sError),
                    pOnlParOut->sError);

    return (bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN fnIoctlDoNetProfi(  DEVICE_EXTENSION*           pDevExtIn,
                            uPNDEV_NP_DO_NETPROFI_REQ_IN*  pOnlParIn,
                            uPNDEV_NP_DO_NETPROFI_REQ_OUT* pOnlParOut)
{
BOOLEAN bResult = FALSE;

    // Note:
    //  - at Linux this Service is not supported

    // set ErrorString
    //  - not changed by DLL
    fnBuildString(  _TEXT(__FUNCTION__),
                    _TEXT("(): OperatingSystem not supported."),
                    NULL,
                    _countof(pOnlParOut->sError),
                    pOnlParOut->sError);

    return (bResult);
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  connect interrupt
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN fnConnectInt(   DEVICE_EXTENSION*   pDevExtIn,
                        const UINT32        lDimErrorIn,
                        _TCHAR*             sErrorOut)
{
BOOLEAN                 bResult = FALSE;
NTSTATUS                lResult = STATUS_UNSUCCESSFUL;
WDF_INTERRUPT_CONFIG    oConfigInterrupt;

    // Note:
    //  - function configures and creates the WDFINTERRUPT object
    //  - function is typically called by EvtDeviceAdd callback
    //  - unlike WDM, WDF driver should create interrupt object in EvtDeviceAdd and let the framework do the resource parsing and registration of ISR with the kernel
    //  - framework connects the interrupt after invoking the fnEvtDeviceD0Entry callback and disconnect before invoking fnEvtDeviceD0Exit
    //  - fnEvtIntEnable is called after the interrupt is connected and fnEvtIntDisable before the interrupt is disconnected

    WDF_INTERRUPT_CONFIG_INIT( &oConfigInterrupt,
                               fnEvtIsr,
                               fnEvtDpc);

    oConfigInterrupt.EvtInterruptEnable  = fnEvtIntEnable;
    oConfigInterrupt.EvtInterruptDisable = fnEvtIntDisable;

    // automatic synchronization of DPCs
    //  - on a multiprocessor system there can be more than one DPCs running simultaneously on multiple processors
    //  - setting of AutomaticSerialization:
    //      -> SpinLock is set by framework (synchronizes access to driver data from code that runs at IRQL <= DISPATCH_LEVEL)
    //      -> does not synchronize an EvtInterruptIsr callback function with other callback functions
    // klsp:
    //  - use own SpinLock for more flexibility
    //oConfigInterrupt.AutomaticSerialization = TRUE;

    lResult = WdfInterruptCreate(pDevExtIn->hDeviceOs,
                                 &oConfigInterrupt,
                                 WDF_NO_OBJECT_ATTRIBUTES,
                                 &pDevExtIn->hInterrupt);

    if  (lResult != STATUS_SUCCESS)
        // error
    {
    _TCHAR sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

        // set ErrorDetail
        _RtlStringCchPrintf(sErrorDetail,
                            _countof(sErrorDetail),
                            _TEXT("(): Error at WdfInterruptCreate() (status=%!STATUS!)!"),
                            lResult);

        // set ErrorString
        fnBuildString(  _TEXT(__FUNCTION__),
                        sErrorDetail,
                        NULL,
                        lDimErrorIn,
                        sErrorOut);
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
BOOLEAN fnConnectInt(   DEVICE_EXTENSION*       pDevExtIn,
                        const UINT32            lDimErrorIn,
                        _TCHAR*                 sErrorOut)
{
BOOLEAN bResult = FALSE;
UINT32  i = 0;

    // preset
    bResult = TRUE;

    // search for a free IntVector
    //  - ADN_INT_SYS_NR...ADN_INT_MAX_NR (32...255)
    //  - for avoiding conflicts with other drivers we start at ADN_INT_PCI_INTA (= random define of Adonis!)
    {
        for (i = ADN_INT_PCI_INTA; i < ADN_INT_MAX_NR; i++)
            // all possible IntVectors
        {
            if  (fnRegisterIsr( pDevExtIn,
                                i,
                                lDimErrorIn,
                                sErrorOut))
                // registering ISR for INTA ok
            {
                pDevExtIn->lIntVector = i;

                // leave loop
                break;
            }
        }
    }

    if  (!bResult)
        // error
    {
        // set ErrorString
        fnBuildString(  _TEXT(__FUNCTION__),
                        _TEXT("(): Error at registering ISR!"),
                        NULL,
                        lDimErrorIn,
                        sErrorOut);
    }
    else
    {
        // enable IntLine at IO-APIC
        //  - clear its MaskBit
        ADN_IRQ_ENABLE(pDevExtIn->lIntLine);
    }

    return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN fnConnectInt(   DEVICE_EXTENSION*       pDevExtIn,
                        const UINT32            lDimErrorIn,
                        _TCHAR*                 sErrorOut)
{
BOOLEAN bResult = FALSE;

    bResult = fnRegisterIsr(    pDevExtIn,
                                pDevExtIn->pPciDev->irq,
                                lDimErrorIn,
                                sErrorOut);

    if  (!bResult)
        // error
    {
        // set ErrorString
        fnBuildString(  _TEXT(__FUNCTION__),
                        _TEXT("(): Error at registering ISR!"),
                        NULL,
                        lDimErrorIn,
                        sErrorOut);
    }
//    else
//    {
		// enable IntLine at Interruptcontroller
    	// not necessary, because no disable will be done during initialization
//        enable_irq(pDevExtIn->pPciDev->irq);
//    }

    return(bResult);
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  disconnect interrupt
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN fnDisconnectInt(DEVICE_EXTENSION*   pDevExtIn,
                        const UINT32        lDimErrorIn,
                        _TCHAR*             sErrorOut)
{
BOOLEAN bResult = FALSE;

    // success
    bResult = TRUE;

    return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
BOOLEAN fnDisconnectInt(DEVICE_EXTENSION*   pDevExtIn,
                        const UINT32        lDimErrorIn,
                        _TCHAR*             sErrorOut)
{
BOOLEAN bResult = FALSE;

    // disable IntLine at IO-APIC
    ADN_IRQ_DISABLE(pDevExtIn->lIntLine);

    if  (adn_irq_unregister(pDevExtIn->lIntVector) != ADN_OK)
        // error at unregistering ISR for INTA
    {
        // set ErrorString
        fnBuildString(  _TEXT(__FUNCTION__),
                        _TEXT("(): Error at adn_irq_unregister()!"),
                        NULL,
                        lDimErrorIn,
                        sErrorOut);
    }
    else
    {
        // success
        bResult = TRUE;
    }

    return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX) && defined (PNDEV_DRIVER_SYS)
//------------------------------------------------------------------------
BOOLEAN fnDisconnectInt(DEVICE_EXTENSION*   pDevExtIn,
                        const UINT32        lDimErrorIn,
                        _TCHAR*             sErrorOut)
{
BOOLEAN bResult = FALSE;

//    // disable IntLine at IO-APIC
//    disable_irq_nosync(pDevExtIn->pPciDev->irq);

    free_irq(pDevExtIn->pPciDev->irq, (void*) pDevExtIn);

    // success
    bResult = TRUE;

    return(bResult);
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  register ISR
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
BOOLEAN fnRegisterIsr(  DEVICE_EXTENSION*   pDevExtIn,
                        const UINT32        lIntVectorIn,
                        const UINT32        lDimErrorIn,
                        _TCHAR*             sErrorOut)
{
BOOLEAN bResult     = FALSE;
int32_t lResultTmp  = ADN_ERROR;

    // configure IO-APIC for desired IntLine
    //  - set IntVector of the local APIC that should be triggered
    //  - PCI interrupts are LevelTriggered / ActiveLow
    adn_irq_ioapic_setup(   pDevExtIn->lIntLine,
                            lIntVectorIn,
                            1,                      // 0: EdgeTriggered     1: LevelTriggered
                            1);                     // 0: ActiveHigh        1: ActiveLow

    // register ISR for selected IntVector
    lResultTmp = adn_irq_register(  lIntVectorIn,
                                    &fnEvtIsr,              // function
                                    0,                      // 1st FunctionArg
                                    (void*) pDevExtIn);     // 2nd FunctionArg

    if  (lResultTmp != ADN_OK)
        // error
    {
        // set ErrorString
        fnBuildString(  _TEXT(__FUNCTION__),
                        _TEXT("(): Error at adn_irq_register()!"),
                        NULL,
                        lDimErrorIn,
                        sErrorOut);
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
BOOLEAN fnRegisterIsr(  DEVICE_EXTENSION*   pDevExtIn,
                        const UINT32        lIntVectorIn,
                        const UINT32        lDimErrorIn,
                        _TCHAR*             sErrorOut)
{
BOOLEAN     bResult         = FALSE;
int32_t     lResultTmp      = LINUX_ERROR;
PNDEV_CHAR  sIntName[PNDEV_SIZE_STRING_BUF_SHORT]   = {0};

    // ToDo: MK string zusammenbauen

    RtlStringCchPrintfA(sIntName,
                        _countof(sIntName),
                        "%u,%u,%u",
                        pDevExtIn->uPciLoc.uNo.lBus,
                        pDevExtIn->uPciLoc.uNo.lDevice,
                        pDevExtIn->uPciLoc.uNo.lFunction);

    // register ISR for selected IntVector
    lResultTmp = request_irq(   lIntVectorIn,           // interrupt number
                                fnEvtIsr,               // function
                                IRQF_SHARED,            // fast interrupts: are executed with interrupts disabled
                                pDevExtIn->sPciLocShort,// name of interrupt owner in /proc/interrupts, make name unique
                                (void*) pDevExtIn);     // identifier to deregister shared interrupts (also usefull if not shared)

    if  (lResultTmp != LINUX_OK)
        // error
    {
        // set ErrorString
        fnBuildString(  _TEXT(__FUNCTION__),
                        _TEXT("(): Error at request_irq()!"),
                        NULL,
                        lDimErrorIn,
                        sErrorOut);
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

//************************************************************************
//  D e s c r i p t i o n :
//
//  queue DPC
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN fnQueueDpc( DEVICE_EXTENSION*   pDevExtIn,
                    HANDLE              hOsParIn)
{
BOOLEAN bResult = FALSE;

    // Note:
    //  - method returns FALSE if the callback function was previously queued and has not executed

    if  (WdfInterruptQueueDpcForIsr((WDFINTERRUPT) hOsParIn))
        // queuing DPC ok
    {
        // success
        bResult = TRUE;
    }

    return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
BOOLEAN fnQueueDpc( DEVICE_EXTENSION*   pDevExtIn,
                    HANDLE              hOsParIn)
{
BOOLEAN bResult = FALSE;

    // Note:
    //  - at Adonis this function will never be called

    return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN fnQueueDpc( DEVICE_EXTENSION*   pDevExtIn,
                    HANDLE              hOsParIn)
{
BOOLEAN bResult = FALSE;

    // Note:
    //  - at Linux this function will never be called

    return(bResult);
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  allocate memory
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
void* fnAllocMemNonPaged(   size_t  lSizeIn,
                            ULONG   lTagIn)
{
void* pResult = NULL;

    pResult = ExAllocatePoolWithTag(NonPagedPool,
                                    lSizeIn,
                                    lTagIn);

    return(pResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
void* fnAllocMemNonPaged(   size_t  lSizeIn,
                            ULONG   lTagIn)
{
void* pResult = NULL;

    pResult = malloc(lSizeIn);

    return(pResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
void* fnAllocMemNonPaged(   size_t  lSizeIn,
                            ULONG   lTagIn)
{
void* pResult = NULL;

    pResult = __kmalloc(lSizeIn,
                        GFP_ATOMIC);

    return(pResult);
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  free memory
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
void fnFreeMemNonPaged( void*   pMemIn,
                        ULONG   lTagIn)
{
    ExFreePoolWithTag(  pMemIn,
                        lTagIn);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
void fnFreeMemNonPaged( void*   pMemIn,
                        ULONG   lTagIn)
{
    free(pMemIn);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
void fnFreeMemNonPaged( void*   pMemIn,
                        ULONG   lTagIn)
{
    kfree(pMemIn);
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  map physical addresses to non-paged virtual memory
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN fnMapMemPhyToVirtual(   DEVICE_EXTENSION*           pDevExtIn,
                                uPNDEV_HW_RES_SINGLE_KM*    pMemKmInOut,
                                const UINT32                lBarIndexIn,
                                const UINT32                lDimErrorIn,
                                _TCHAR*                     sErrorOut)
{
BOOLEAN bResult = FALSE;
char    sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

    #ifdef PNDEV_DEVELOPMENT
        // set TraceDetail
        RtlStringCchPrintfA(sTraceDetail,
                            _countof(sTraceDetail),
                            "PnDev[%s]: PhyAdr=%llx, size=0x%x",
                            pDevExtIn->sPciLocShortAscii,
                            pMemKmInOut->uPhyAdr.QuadPart,
                            pMemKmInOut->lSize);

        // set trace
        fnSetTrace( ePNDEV_TRACE_LEVEL_INFO,
                    ePNDEV_TRACE_CONTEXT_BASIC,
                    FALSE,                      // logging
                    sTraceDetail);
    #endif

    if  (pMemKmInOut->lSize == 0)
        // memory not used
    {
        // success
        bResult = TRUE;
    }
    else
    {
        // map PhyAdr
        //  - non cached
        pMemKmInOut->pVirtualAdr = (volatile UINT8*) MmMapIoSpace(  pMemKmInOut->uPhyAdr,
                                                                    pMemKmInOut->lSize,
                                                                    MmNonCached);

        if  (pMemKmInOut->pVirtualAdr == NULL)
            // error
        {
        _TCHAR sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

            // set ErrorDetail
            _RtlStringCchPrintf(sErrorDetail,
                                _countof(sErrorDetail),
                                _TEXT("(): Unable to map physical memory 0x%llx, size 0x%x to virtual memory!"),
                                pMemKmInOut->uPhyAdr.QuadPart,
                                pMemKmInOut->lSize);

            // set ErrorString
            fnBuildString(  _TEXT(__FUNCTION__),
                            sErrorDetail,
                            NULL,
                            lDimErrorIn,
                            sErrorOut);
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
BOOLEAN fnMapMemPhyToVirtual(   DEVICE_EXTENSION*           pDevExtIn,
                                uPNDEV_HW_RES_SINGLE_KM*    pMemKmInOut,
                                const UINT32                lBarIndexIn,
                                const UINT32                lDimErrorIn,
                                _TCHAR*                     sErrorOut)
{
BOOLEAN bResult = FALSE;

    if  (pMemKmInOut->lSize == 0)
        // memory not used
    {
        // success
        bResult = TRUE;
    }
    else
    {
        // map PhyAdr to UserMode
        //  -> access by KernelMode automatically
        pMemKmInOut->pVirtualAdr = adn_mem_mmap_add((void*) pMemKmInOut->uPhyAdr.u.LowPart,
                                                    NULL,
                                                    pMemKmInOut->lSize,
                                                    (PROT_READ | PROT_WRITE),
                                                    MAP_SHARED,
                                                    ADN_MEM_UNCACHED);

        if  (pMemKmInOut->pVirtualAdr == NULL)
            // error
        {
        _TCHAR sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

            // set ErrorDetail
            _RtlStringCchPrintf(sErrorDetail,
                                _countof(sErrorDetail),
                                _TEXT("(): Unable to map physical memory 0x%x, size 0x%x to virtual memory!"),
                                pMemKmInOut->uPhyAdr.QuadPart,
                                pMemKmInOut->lSize);

            // set ErrorString
            fnBuildString(  _TEXT(__FUNCTION__),
                            sErrorDetail,
                            NULL,
                            lDimErrorIn,
                            sErrorOut);
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
BOOLEAN fnMapMemPhyToVirtual(   DEVICE_EXTENSION*           pDevExtIn,
                                uPNDEV_HW_RES_SINGLE_KM*    pMemKmInOut,
                                const UINT32                lBarIndexIn,
                                const UINT32                lDimErrorIn,
                                _TCHAR*                     sErrorOut)
{
BOOLEAN bResult = FALSE;

    if  (pMemKmInOut->lSize == 0)
        // memory not used
    {
        // success
        bResult = TRUE;
    }
    else
    {
        // map PhyAdr to UserMode
        //  -> access by KernelMode automatically
        pMemKmInOut->pVirtualAdr = pci_ioremap_bar(   pDevExtIn->pPciDev,
                                                    lBarIndexIn);

        if  (pMemKmInOut->pVirtualAdr == NULL)
            // error
        {
        _TCHAR sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

            // set ErrorDetail
            _RtlStringCchPrintf(sErrorDetail,
                                _countof(sErrorDetail),
                                _TEXT("(): Unable to map Bar%d physical memory 0x%Lx, size 0x%x to virtual memory!"),
                                lBarIndexIn,
                                pMemKmInOut->uPhyAdr.QuadPart,
                                pMemKmInOut->lSize);

            // set ErrorString
            fnBuildString(  _TEXT(__FUNCTION__),
                            sErrorDetail,
                            NULL,
                            lDimErrorIn,
                            sErrorOut);
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
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  release virtual memory
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
void fnReleaseMemVirtual(   DEVICE_EXTENSION*           pDevExtIn,
                            uPNDEV_HW_RES_SINGLE_KM*    pMemKmInOut)
{
    if  (pMemKmInOut->pVirtualAdr != NULL)
        // memory mapped to system space
    {
        // release virtual memory
        MmUnmapIoSpace( (void*) pMemKmInOut->pVirtualAdr,
                        pMemKmInOut->lSize);

        pMemKmInOut->pVirtualAdr = NULL;
    }
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
void fnReleaseMemVirtual(   DEVICE_EXTENSION*           pDevExtIn,
                            uPNDEV_HW_RES_SINGLE_KM*    pMemKmInOut)
{
    if  (pMemKmInOut->pVirtualAdr != NULL)
        // memory mapped to system space
    {
        // release virtual memory
        adn_mem_mmap_remove((void*) pMemKmInOut->pVirtualAdr,
                            pMemKmInOut->lSize);

        pMemKmInOut->pVirtualAdr = NULL;
    }
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
void fnReleaseMemVirtual(   DEVICE_EXTENSION*           pDevExtIn,
                            uPNDEV_HW_RES_SINGLE_KM*    pMemKmInOut)
{

    if  (pMemKmInOut->pVirtualAdr != NULL)
        // memory mapped to system space
    {
        // release virtual memory
        pci_iounmap(pDevExtIn->pPciDev,
                    (void*) pMemKmInOut->pVirtualAdr);

        pMemKmInOut->pVirtualAdr = NULL;
    }

}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  map virtual memory addresses to UserMode
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN fnMapMemVirtualToUser(  DEVICE_EXTENSION*           pDevExtIn,
                                const void*                 pVirtualAdrIn,
                                const UINT32                lSizeMemIn,
                                const UINT32                lDimErrorIn,
                                uPNDEV_HW_RES_SINGLE_UM*    pMemUmOut,
                                _TCHAR*                     sErrorOut)
{
BOOLEAN bResult = FALSE;

    // preset OutputParameter
    RtlZeroMemory(  pMemUmOut,
                    sizeof(uPNDEV_HW_RES_SINGLE_UM));

    if  (lSizeMemIn == 0)
        // memory not used
    {
        // success
        bResult = TRUE;
    }
    else
    {
    UINT32  lSizePage       = PAGE_SIZE;
    UINT32  lSizeMdl        = sizeof(MDL);
    UINT32  lSizeUlongPtr   = sizeof(ULONG_PTR);
    UINT64  lMaxSize        = 0;
    _TCHAR  sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

        // compute MaxSize
        //  - Windows XP:       PAGE_SIZE * (65535 - sizeof(MDL)) / sizeof(ULONG_PTR)
        //                      (0x1000 * (65535 - 0x1c)) / 4 = 0x03ff_8c00 = 63,97MB
        //  - Windows Vista:    2 Gigabytes - PAGE_SIZE
        //  - >= Windows 7:     4 Gigabytes - PAGE_SIZE
        {
                // selected BuildEnvironment >= Win7
                lMaxSize = MAXUINT32 - lSizePage + 1;
        }

        if  (lSizeMemIn >= lMaxSize)
            // invalid size
        {
            // set ErrorDetail
            _RtlStringCchPrintf(sErrorDetail,
                                _countof(sErrorDetail),
                                _TEXT("(): Invalid MemSize (real: 0x%x, max: 0x%x)!"),
                                lSizeMemIn,
                                lMaxSize);

            // set ErrorString
            fnBuildString(  _TEXT(__FUNCTION__),
                            sErrorDetail,
                            NULL,
                            lDimErrorIn,
                            sErrorOut);
        }
        else
        {
            // allocate a MDL large enough to map a buffer
            pMemUmOut->uMdl.pPtr = (void*) IoAllocateMdl(   (void*) pVirtualAdrIn,  // base virtual address of the buffer the MDL is to describe
                                                            lSizeMemIn,             // length in bytes of the buffer the MDL is to describe
                                                            FALSE,                  // FALSE if no IRP is associated with the MDL
                                                            FALSE,                  // this can be set to TRUE only by highest-level drivers that are called in the context of the thread that originates the I/O request
                                                            NULL);                  // ptr to an IRP to be associated with the MDL

            if  (pMemUmOut->uMdl.pPtr == NULL)
                // error
            {
                // set ErrorString
                fnBuildString(  _TEXT(__FUNCTION__),
                                _TEXT("(): Error at IoAllocateMdl()!"),
                                NULL,
                                lDimErrorIn,
                                sErrorOut);
            }
            else
            {
            BOOLEAN bError = FALSE;

                // Note:
                //  - IoAllocateMdl can be used by a driver that needs to break a buffer into pieces, each mapped by a separate MDL, or to map a driver-allocated buffer.
                //  - The driver should call MmBuildMdlForNonPagedPool with the MDL allocated by this call to set up an MDL describing a driver-allocated buffer in NonPagedPool.

                // build MDL that specifies a virtual memory buffer in NonPagedPool, and updates it to describe the underlying physical pages.
                MmBuildMdlForNonPagedPool((PMDL) pMemUmOut->uMdl.pPtr);

                // don't use MmProbeAndLockPages() because pages are already locked (NonPagedPool!)
                //  - otherwise exception
/*
                try
                {
                    // probe specified virtual memory pages, makes them resident, and locks them in memory
                    MmProbeAndLockPages(pMemUmOut->pMdl,
                                        UserMode,
                                        IoModifyAccess);
                }
                except(EXCEPTION_EXECUTE_HANDLER)
                {
                    // error handling code
                    bError = TRUE;

                    // set ErrorString
                    fnBuildString(  _TEXT(__FUNCTION__),
                                    _TEXT("(): Exception at MmProbeAndLockPages()!"),
                                    NULL,
                                    lDimErrorIn,
                                    sErrorOut);
                }
*/
                if  (!bError)
                    // success
                {
                    // Note:
                    //  - The caller must already have probed and locked the MDL by using the MmProbeAndLockPages routine
                    //  - NO, see above

                    try
                    {
                        // maps the physical pages that are described by an MDL
                        pMemUmOut->uAdrUser.pPtr = MmMapLockedPagesSpecifyCache((PMDL) pMemUmOut->uMdl.pPtr,
														                        UserMode,
															                    MmNonCached,
																                NULL,                   // BaseAddress
																	            FALSE,                  // BugCheckOnFailure: if FALSE routine returns NULL in error case
																		        NormalPagePriority);    // mapping request can fail if the system is very low on resources
                    }
                    except(EXCEPTION_EXECUTE_HANDLER)
                    {
                        // error handling code
                        bError = TRUE;

                        // set ErrorString
                        fnBuildString(  _TEXT(__FUNCTION__),
                                        _TEXT("(): Exception at MmMapLockedPagesSpecifyCache()!"),
                                        NULL,
                                        lDimErrorIn,
                                        sErrorOut);
                    }

                    if  (pMemUmOut->uAdrUser.pPtr == NULL)
                        // error
                    {
                        // set ErrorDetail
                        _RtlStringCchPrintf(sErrorDetail,
                                            _countof(sErrorDetail),
                                            _TEXT("(): Unable to map virtual memory 0x%llx, size 0x%x to UserMode!"),
                                            pVirtualAdrIn,
                                            lSizeMemIn);

                        // set ErrorString
                        fnBuildString(  _TEXT(__FUNCTION__),
                                        sErrorDetail,
                                        NULL,
                                        lDimErrorIn,
                                        sErrorOut);
                    }
                    else
                    {
                        // success
                        bResult = TRUE;

                        // return
                        pMemUmOut->lSize = lSizeMemIn;
                    }
                }
            }
        }
    }

    return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
BOOLEAN fnMapMemVirtualToUser(  DEVICE_EXTENSION*           pDevExtIn,
                                const void*                 pVirtualAdrIn,
                                const UINT32                lSizeMemIn,
                                const UINT32                lDimErrorIn,
                                uPNDEV_HW_RES_SINGLE_UM*    pMemUmOut,
                                _TCHAR*                     sErrorOut)
{
BOOLEAN bResult = FALSE;

    if  (lSizeMemIn == 0)
        // memory not used
    {
        // success
        bResult = TRUE;
    }
    else
    {
        // success
        bResult = TRUE;

        // Note:
        //  - all BARs are already mapped to UserMode
        //  - see fnMapMemPhyToVirtual()

        // return
        pMemUmOut->uAdrUser.pPtr = (volatile UINT8*) pVirtualAdrIn;
        pMemUmOut->lSize    = lSizeMemIn;
    }

    return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN fnMapMemVirtualToUser(  DEVICE_EXTENSION*           pDevExtIn,
                                const void*                 pVirtualAdrIn,
                                const UINT32                lSizeMemIn,
                                const UINT32                lDimErrorIn,
                                uPNDEV_HW_RES_SINGLE_UM*    pMemUmOut,
                                _TCHAR*                     sErrorOut)
{
BOOLEAN bResult = FALSE;

    // will never be called in Linux

    return(bResult);
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  release memory mapped to UserMode
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
void fnReleaseMemUser(  DEVICE_EXTENSION*               pDevExtIn,
                        const uPNDEV_HW_RES_SINGLE_UM*  pMemUmIn)
{
    if  (pMemUmIn->uAdrUser.pPtr)
        // valid ptr
    {
        // Note:
        //  - if the call to MmMapLockedPagesSpecifyCache specified user mode, the caller must be in the context of the original process before calling MmUnmapLockedPages
        //  - otherwise the unmapping operation could delete the address range of a random process

        // release mapping set up by a preceding call MmMapLockedPagesSpecifyCache
        MmUnmapLockedPages( (void*) pMemUmIn->uAdrUser.pPtr,
                            (PMDL) pMemUmIn->uMdl.pPtr);
    }

    if  (pMemUmIn->uMdl.pPtr != NULL)
        // valid MDL
    {
        if  (((PMDL) (pMemUmIn->uMdl.pPtr))->MdlFlags & MDL_PAGES_LOCKED)
            // Flags are set before calling UnlockPages
        {
            // unlocks physical pages described by a given MDL
            MmUnlockPages((PMDL) pMemUmIn->uMdl.pPtr);
        }

        // free MDL
        IoFreeMdl((PMDL) pMemUmIn->uMdl.pPtr);
    }

    // reset
    RtlZeroMemory(  (void*) pMemUmIn,
                    sizeof(uPNDEV_HW_RES_SINGLE_UM));
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
void fnReleaseMemUser(  DEVICE_EXTENSION*               pDevExtIn,
                        const uPNDEV_HW_RES_SINGLE_UM*  pMemUmIn)
{
    // reset
    RtlZeroMemory(  (void*) pMemUmIn,
                    sizeof(uPNDEV_HW_RES_SINGLE_UM));
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
void fnReleaseMemUser(  DEVICE_EXTENSION*               pDevExtIn,
                        const uPNDEV_HW_RES_SINGLE_UM*  pMemUmIn)
{
    // munmap will be called in Usermode

    // reset
    RtlZeroMemory(  (void*) pMemUmIn,
                    sizeof(uPNDEV_HW_RES_SINGLE_UM));
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  start WaitTime of current thread
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN fnDelayThread(  DEVICE_EXTENSION*   pDevExtIn,
                        const UINT32        lTimeMsecIn,
                        const UINT32        lDimErrorIn,
                        _TCHAR*             sErrorOut)
{
BOOLEAN bResult             = FALSE;
UINT32  lTimeDesiredMsec    = 0;

    // update desired WaitTime
    lTimeDesiredMsec = lTimeMsecIn * pDevExtIn->lTimeScaleFactor;

    if  (lTimeDesiredMsec == 0)
        // no WaitTime
    {
        // success
        bResult = TRUE;
    }
    else
    {
    LARGE_INTEGER uValueTimer;

        // wait
        //  - KeDelayExecutionThread must be running at IRQL = PASSIVE_LEVEL
        //  - FALSE:    thread will not be alerted (Warning, Alarm)
        //  - Timeout:  n * 100nsec (negative: relative time)
        //  - if negative: casting to INT64 necessary, otherwise unsigned -> HighPart=0
        uValueTimer.QuadPart = -((INT64) (10000 * lTimeDesiredMsec));

        if  (KeDelayExecutionThread(KernelMode,
                                    FALSE,
                                    &uValueTimer) != STATUS_SUCCESS)
            // error
        {
            // set ErrorString
            fnBuildString(  _TEXT(__FUNCTION__),
                            _TEXT("(): Error at KeDelayExecutionThread()!"),
                            NULL,
                            lDimErrorIn,
                            sErrorOut);
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
BOOLEAN fnDelayThread(  DEVICE_EXTENSION*   pDevExtIn,
                        const UINT32        lTimeMsecIn,
                        const UINT32        lDimErrorIn,
                        _TCHAR*             sErrorOut)
{
BOOLEAN bResult             = FALSE;
UINT32  lTimeDesiredMsec    = 0;

    // update desired WaitTime
    lTimeDesiredMsec = lTimeMsecIn * pDevExtIn->lTimeScaleFactor;

    if  (lTimeDesiredMsec == 0)
        // no WaitTime
    {
        // success
        bResult = TRUE;
    }
    else
    {
    struct timespec uTimeSpec;

        uTimeSpec.tv_sec    = lTimeDesiredMsec / 1000u;                         // sec = msec / 1000
        uTimeSpec.tv_nsec   = ((lTimeDesiredMsec % 1000u) * 1000u * 1000u);     // nsec = msec*1000*1000  %1000 to eliminate the secs

        // Note:
        // - wraparound and overflow can not happen due to the division by 1000 at the secs and the % 1000 at the nsecs

        if  (nanosleep( &uTimeSpec,
                        NULL) != ADN_OK)
          // error
        {
          // set ErrorString
          fnBuildString(  _TEXT(__FUNCTION__),
                          _TEXT("(): Error at nanosleep()!"),
                          NULL,
                          lDimErrorIn,
                          sErrorOut);
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
BOOLEAN fnDelayThread(  DEVICE_EXTENSION*   pDevExtIn,
                        const UINT32        lTimeMsecIn,
                        const UINT32        lDimErrorIn,
                        _TCHAR*             sErrorOut)
{
BOOLEAN bResult             = FALSE;
UINT32  lTimeDesiredMsec    = 0;

    // update desired WaitTime
    lTimeDesiredMsec = lTimeMsecIn * pDevExtIn->lTimeScaleFactor;

    if  (lTimeDesiredMsec == 0)
        // no WaitTime
    {
        // success
        bResult = TRUE;
    }
    else
    {
        // wait msec
        msleep(lTimeDesiredMsec);

        // success
        bResult = TRUE;
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
//  get clock
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------

void fnGetClock(uPNDEV_CLOCK* pClockOut)
{
    // Note:
    //  - C is really able to return a struct!
    //  - but not optimal performance because of copying elements!

    // read PerformanceCtr and PerformanceFreq
    *pClockOut = KeQueryPerformanceCounter(NULL);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
void fnGetClock(uPNDEV_CLOCK* pClockOut)
{
    if  (clock_gettime( CLOCK_MONOTONIC,
                        pClockOut) != ADN_OK)
        // error
    {
        pClockOut->tv_sec   = 0;
        pClockOut->tv_nsec  = 0;
    }
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
void fnGetClock(uPNDEV_CLOCK* pClockOut)
{
    getrawmonotonic(pClockOut);
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  compute RunTime in usec
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
UINT32 fnComputeRunTimeUsec(uPNDEV_CLOCK* pClockStartIn)
{
UINT32          lResult     = 0;
UINT64          lRunTime    = 0;    // must be UINT64, otherwise possible overflow!
LARGE_INTEGER   uClockStop;
LARGE_INTEGER   uFreq;

    // read PerformanceCtr and PerformanceFreq
    uClockStop = KeQueryPerformanceCounter(&uFreq);

    // compute Runtime (    sec:    ticks / Freq
    //                      usec:   (ticks * 1000 * 1000) / Freq)
    lRunTime  = uClockStop.QuadPart - pClockStartIn->QuadPart;
    lRunTime *= (1000 * 1000);
    lRunTime /= (UINT32) uFreq.QuadPart;

    // return
    lResult = (UINT32) lRunTime;

    return(lResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
UINT32 fnComputeRunTimeUsec(uPNDEV_CLOCK* pClockStartIn)
{
UINT32          lResult = 0;
struct timespec uClockStop;

    if  (clock_gettime( CLOCK_MONOTONIC,
                        &uClockStop) == ADN_OK)
        // getting clock ok
    {
    UINT64 lTimeStart_Usec  = 0;
    UINT64 lTimeStop_Usec   = 0;

        // compute StartTime in usec
        lTimeStart_Usec = (pClockStartIn->tv_sec * 1000 * 1000) + (pClockStartIn->tv_nsec / 1000);

        // compute StopTime
        lTimeStop_Usec = (uClockStop.tv_sec * 1000 * 1000) + (uClockStop.tv_nsec / 1000);

        // compute Runtime in usec
        lResult = (UINT32) (lTimeStop_Usec - lTimeStart_Usec);
    }

    return(lResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
UINT32 fnComputeRunTimeUsec(uPNDEV_CLOCK* pClockStartIn)
{
UINT32          lResult         = 0;
struct timespec uClockStop;

    getrawmonotonic(&uClockStop);

    if  (   (uClockStop.tv_nsec > 0)
        ||  (uClockStop.tv_sec > 0))
        // getting clock ok
    {
    UINT64 lTimeStart_Usec  = 0;
    UINT64 lTimeStop_Usec   = 0;

        // compute StartTime in usec
        lTimeStart_Usec = (pClockStartIn->tv_sec * 1000 * 1000) + (pClockStartIn->tv_nsec / 1000);

        // compute StopTime
        lTimeStop_Usec = (uClockStop.tv_sec * 1000 * 1000) + (uClockStop.tv_nsec / 1000);

        // compute Runtime in usec
        lResult = (UINT32) (lTimeStop_Usec - lTimeStart_Usec);
    }

    return(lResult);
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  set trace
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
void fnSetTrace(const ePNDEV_TRACE_LEVEL    eLevelIn,
                const ePNDEV_TRACE_CONTEXT  eContextIn,
                const BOOLEAN               bLogIn,
                char*                       sTraceIn)
{
UINT32 lLevel = 0;

    if  (bLogIn)
        // logging desired
    {
        /* Unicode format codes (%C, %S, %lc, %ls, %wc, %ws, and %wZ) can only be used with IRQL = PASSIVE_LEVEL */
        DbgPrintEx( DPFLTR_IHVDRIVER_ID,
                    DPFLTR_ERROR_LEVEL,
                    sTraceIn);
    }

    // call TraceEvents
    {
        // Note:
        //  - TraceEvents function is mapped to DoTraceMessage provided by WPP by using a directive in the sources file
        //  - TraceEvents cannot be wrapped by a macro because of precompiling
        //  - Unicode format codes (%wc, %ws) can only be used at IRQL = PASSIVE_LEVEL -> ASCII
        //  - ContextBits are defined at file trace.h
        //  - WPP tracing code is generated by file sources

        if  (eLevelIn == ePNDEV_TRACE_LEVEL_ERROR)
            // TraceLevel = error
        {
            lLevel = TRACE_LEVEL_ERROR;
        }
        else
        {
            lLevel = TRACE_LEVEL_INFORMATION;
        }

        switch  (eContextIn)
                // TraceContext
        {
            case ePNDEV_TRACE_CONTEXT_BASIC:    {TraceEvents(lLevel, DBG_PNP,       "%s", sTraceIn);    break;}
            case ePNDEV_TRACE_CONTEXT_DPC:      {TraceEvents(lLevel, DBG_DPC,       "%s", sTraceIn);    break;}
            case ePNDEV_TRACE_CONTEXT_ISR:      {TraceEvents(lLevel, DBG_INTERRUPT, "%s", sTraceIn);    break;}
            default:
            {
                break;
            }
        }
    }
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
void fnSetTrace(const ePNDEV_TRACE_LEVEL    eLevelIn,
                const ePNDEV_TRACE_CONTEXT  eContextIn,
                const BOOLEAN               bLogIn,
                char*                       sTraceIn)
{
    // call user specific adaption
    fnSetTraceSys(  eLevelIn,
                    eContextIn,
                    bLogIn,
                    sTraceIn);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
void fnSetTrace(const ePNDEV_TRACE_LEVEL    eLevelIn,
                const ePNDEV_TRACE_CONTEXT  eContextIn,
                const BOOLEAN               bLogIn,
                char*                       sTraceIn)
{
    printk(KERN_WARNING "%s\n", sTraceIn);
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  check IRQL
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN fnCheckIrql(const KIRQL lIrqlMaxIn,
                    KIRQL*      pIrqlCurOut)
{
BOOLEAN bResult = FALSE;
KIRQL   lIrql   = PASSIVE_LEVEL;

    // preset OutputParameter
    *pIrqlCurOut = PASSIVE_LEVEL;

    lIrql = KeGetCurrentIrql();

    // return
    *pIrqlCurOut = lIrql;

    if  (lIrql <= lIrqlMaxIn)
        // valid IrqLevel
    {
        // success
        bResult = TRUE;
    }

    return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS) || (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN fnCheckIrql(const KIRQL lIrqlMaxIn,
                    KIRQL*      pIrqlCurOut)
{
BOOLEAN bResult = FALSE;

    // success
    bResult = TRUE;

    // return
    *pIrqlCurOut = lIrqlMaxIn;

    return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  get ResList of ConfigSpace
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN fnGetResListConfigSpace(DEVICE_EXTENSION*   pDevExtIn,
                                HANDLE              hOsParIn,               // hResTranslatedIn
                                const UINT32        lFirstBarIn,
                                const UINT32        lDimErrorIn,
                                _TCHAR*             sErrorOut)
{
BOOLEAN bResult = FALSE;
UINT32  lCtrRes = 0;

    // get number of resources
    lCtrRes = WdfCmResourceListGetCount((WDFCMRESLIST) hOsParIn);

    if  (lCtrRes == 0)
        // no resources
    {
        // set ErrorString
        fnBuildString(  _TEXT(__FUNCTION__),
                        _TEXT("(): No resources exist!"),
                        NULL,
                        lDimErrorIn,
                        sErrorOut);
    }
    else
    {
    PCM_PARTIAL_RESOURCE_DESCRIPTOR pResDescriptor;
    UINT32  lBarIndex = 0;
    UINT32  i = 0;

        // preset
        bResult     = TRUE;
        lBarIndex   = lFirstBarIn;

        // parse the resource list

        for (i = 0; ((i < lCtrRes) && bResult); i++)
            // all resources
        {
            // get current ResDescriptor
            pResDescriptor = WdfCmResourceListGetDescriptor((WDFCMRESLIST) hOsParIn,
                                                            i);

            if  (pResDescriptor == NULL)
                // error
            {
                bResult = FALSE;

                // set ErrorString
                fnBuildString(  _TEXT(__FUNCTION__),
                                _TEXT("(): Error at WdfCmResourceListGetDescriptor()!"),
                                NULL,
                                lDimErrorIn,
                                sErrorOut);
            }
            else
            {
                switch (pResDescriptor->Type)
                {
                    case CmResourceTypePort:
                    {
                        // error, but ignore it
                        bResult = TRUE;

                        // set ErrorString
                        fnBuildString(  _TEXT(__FUNCTION__),
                                        _TEXT("(): Unexpected CmResourceTypePort!"),
                                        NULL,
                                        lDimErrorIn,
                                        sErrorOut);

                        break;
                    }
                    case CmResourceTypeInterrupt:
                    {
                        if  ((pResDescriptor->Flags & CM_RESOURCE_INTERRUPT_MESSAGE) == CM_RESOURCE_INTERRUPT_MESSAGE)
                            // MessageSignaledInt (MSI)
                        {
                            // error
                            bResult = FALSE;

                            // set ErrorString
                            fnBuildString(  _TEXT(__FUNCTION__),
                                            _TEXT("(): Unexpected message signaled interrupt (MSI)!"),
                                            NULL,
                                            lDimErrorIn,
                                            sErrorOut);
                        }
                        else    // LegacyInt
                        {
                        char sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

                            // set TraceDetail
                            RtlStringCchPrintfA(sTraceDetail,
                                                _countof(sTraceDetail),
                                                "PnDev[%s]: IRQ: IntLevel=%u, IntVector=%u, IntAffinity=0x%x",
                                                pDevExtIn->sPciLocShortAscii,
                                                pResDescriptor->u.Interrupt.Level,
                                                pResDescriptor->u.Interrupt.Vector,
                                                pResDescriptor->u.Interrupt.Affinity);

                            // set trace
                            fnSetTrace( ePNDEV_TRACE_LEVEL_INFO,
                                        ePNDEV_TRACE_CONTEXT_BASIC,
                                        TRUE,                       // logging
                                        sTraceDetail);
                        }

                        break;
                    }
                    case CmResourceTypeMemory:
                    {
                        switch  (lBarIndex)
                                // BarIndex
                        {
                            case 0:
                            case 1:
                            case 2:
                            case 3:
                            case 4:
                            case 5:
                            {
                                // store at DeviceExtension
                                pDevExtIn->uHwResKm.uBar[lBarIndex].uPhyAdr.QuadPart    = pResDescriptor->u.Memory.Start.QuadPart;
                                pDevExtIn->uHwResKm.uBar[lBarIndex].lSize               = pResDescriptor->u.Memory.Length;

                                break;
                            }
                            default:
                            {
                            _TCHAR sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

                                // error
                                bResult = FALSE;

                                // set ErrorDetail
                                _RtlStringCchPrintf(sErrorDetail,
                                                    _countof(sErrorDetail),
                                                    _TEXT("(): Invalid BAR number (%u)!"),
                                                    lBarIndex);

                                // set ErrorString
                                fnBuildString(  _TEXT(__FUNCTION__),
                                                sErrorDetail,
                                                NULL,
                                                lDimErrorIn,
                                                sErrorOut);

                                break;
                            }
                        }

                        lBarIndex++;

                        break;
                    }
                    default:
                    {
                        // ignore all other descriptors

                        break;
                    }
                }
            }
        }
    }

    if (bResult)
        // success
    {
        if (pDevExtIn->eBoard == ePNDEV_BOARD_I210)
            // if I210 board
        {
            // Intel I210 board
			// change BAR order, because BAR1 has no size
			// On Intel I210 board BAR2 also has no size
			// BAR4 and BAR5 has no size
            pDevExtIn->uHwResKm.uBar[3] = pDevExtIn->uHwResKm.uBar[1];

            // delete Bar1
            memset( &pDevExtIn->uHwResKm.uBar[1],
                    0,
                    sizeof(uPNDEV_HW_RES_SINGLE_KM));
        }
		else if(pDevExtIn->eBoard == ePNDEV_BOARD_I82574)
			// Intel I82574 board
		{
			// change BAR order, because BAR2 has no size
			// On Intel I82574 board BAR1 exists
			// BAR4 and BAR5 has no size on both boards
			pDevExtIn->uHwResKm.uBar[3] = pDevExtIn->uHwResKm.uBar[2];

			// delete Bar2
			memset(	&pDevExtIn->uHwResKm.uBar[2],
					0,
					sizeof(uPNDEV_HW_RES_SINGLE_KM));
		}
		else if	(	(pDevExtIn->eBoard == ePNDEV_BOARD_FPGA1_HERA)
				|| 	(pDevExtIn->eBoard == ePNDEV_BOARD_TI_OMAP))
			// FPGA1-Hera board
			// TI Omap board
		{
			// change BAR order, because BAR2 has no size
			// On Intel I82574 board BAR1 exists
			// BAR4 and BAR5 has no size on both boards
			pDevExtIn->uHwResKm.uBar[4] = pDevExtIn->uHwResKm.uBar[2];
			pDevExtIn->uHwResKm.uBar[2] = pDevExtIn->uHwResKm.uBar[1];


			// delete Bar1
			memset(	&pDevExtIn->uHwResKm.uBar[1],
					0,
					sizeof(uPNDEV_HW_RES_SINGLE_KM));
		}
    }

    return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
BOOLEAN fnGetResListConfigSpace(DEVICE_EXTENSION*   pDevExtIn,
                                HANDLE              hOsParIn,               // hResTranslatedIn
                                const UINT32        lFirstBarIn,
                                const UINT32        lDimErrorIn,
                                _TCHAR*             sErrorOut)
{
BOOLEAN bResult     = FALSE;
BOOLEAN bGetBarSize = FALSE;
int32_t lResultTmp  = ADN_ERROR;
UINT32  i = 0;

    // preset
    bGetBarSize = TRUE;

    if  (   (pDevExtIn->eBoard == ePNDEV_BOARD_DB_SOC1_PCI)
        ||  (pDevExtIn->eBoard == ePNDEV_BOARD_DB_SOC1_PCIE))
        //      DB-Soc1-PCI board
        // OR   DB-Soc1-PCIe board
    {
        // Note:
        //  - computing a BarSize requires a write access to BarRegister at ConfigSpace and a restoring of it
        //  - meanwhile accesses to this BAR are not allowed!
        //  - if service OpenDriver is not called at the very beginning of the startup phase accesses cannot be avoided!
        //  - solution:
        //      don't compute BarSizes dynamically
        //      use sizes specified by user

        if  (pDevExtIn->eSblSoc == ePNDEV_SBL_S7P_V2)
            // SBL exists
        {
            bGetBarSize = FALSE;
        }
    }

    // preset
    bResult = TRUE;

    for (i = lFirstBarIn; i < PNDEV_DIM_ARRAY_BAR; i++)
        // all desired Bars
    {
    UINT32  lPhyAdr = 0;
    UINT32  lSize   = 0;

        if  (!fnGetBarConfigSpace(  pDevExtIn,
                                    i,
                                    bGetBarSize,
                                    lDimErrorIn,
                                    &lPhyAdr,
                                    &lSize,
                                    sErrorOut))
            // error at getting BarValues of ConfigSpace
        {
            bResult = FALSE;

            // leave loop
            break;
        }
        else
        {
            // store at DeviceExtension
            pDevExtIn->uHwResKm.uBar[i].uPhyAdr.QuadPart    = lPhyAdr;
            pDevExtIn->uHwResKm.uBar[i].lSize               = lSize;
        }
    }

    if  (bResult)
        // success
    {
        if  (   (pDevExtIn->eBoard == ePNDEV_BOARD_DB_SOC1_PCI)
            ||  (pDevExtIn->eBoard == ePNDEV_BOARD_DB_SOC1_PCIE))
            //      DB-Soc1-PCI board
            // OR   DB-Soc1-PCIe board
        {
            // set BarSize
            pDevExtIn->uHwResKm.uBar[0].lSize   = BOARD_SOC1_DEV__BAR0_SIZE_V2;
            pDevExtIn->uHwResKm.uBar[1].lSize   = BOARD_SOC1_DEV__BAR1_SIZE_V2;
            pDevExtIn->uHwResKm.uBar[2].lSize   = BOARD_SOC1_DEV__BAR2_SIZE_V2;
            pDevExtIn->uHwResKm.uBar[3].lSize   = BOARD_SOC1_DEV__BAR3_SIZE_V2;
            pDevExtIn->uHwResKm.uBar[4].lSize   = BOARD_SOC1_DEV__BAR4_SIZE_V2;
            pDevExtIn->uHwResKm.uBar[5].lSize   = BOARD_SOC1_DEV__BAR5_SIZE_V2;
        }
    }

    return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN fnGetResListConfigSpace(DEVICE_EXTENSION*   pDevExtIn,
                                HANDLE              hOsParIn,               // hResTranslatedIn
                                const UINT32        lFirstBarIn,
                                const UINT32        lDimErrorIn,
                                _TCHAR*             sErrorOut)
{
BOOLEAN bResult     = FALSE;
UINT32  lResultTmp  = LINUX_ERROR;

    // preset
    bResult = TRUE;

    // reserve PCI I/O and memory resources
    lResultTmp = pci_request_regions(   pDevExtIn->pPciDev,
                                        "PnDevDrv");

    if  (lResultTmp != LINUX_OK)
        // error
    {
        // set ErrorString
        fnBuildString(  _TEXT(__FUNCTION__),
                        _TEXT("(): Error at pci_request_regions()!"),
                        NULL,
                        lDimErrorIn,
                        sErrorOut);
    }
    else
    {
    UINT32  i = 0;

        // preset
        bResult     = TRUE;

        // parse the resource list
        for (i = lFirstBarIn; i < PNDEV_DIM_ARRAY_BAR; i++)
            // all resources
        {
        UINT32 lResourceFlags = 0;

            // get current Ressource
            lResourceFlags = pci_resource_flags(pDevExtIn->pPciDev,
                                                i);

            if  (lResourceFlags == 0)
                // error
            {
                // ignore the value zero
                // Note:    - some Bars has no resource
                //          - e.g. Eb200P has no resources at Bar 3,4 and 5
                // bResult = FALSE;

                // set ErrorString
                fnBuildString(  _TEXT(__FUNCTION__),
                                _TEXT("(): Error at pci_ressource_flags()!"),
                                NULL,
                                lDimErrorIn,
                                sErrorOut);
            }
            else
            {
                if  (lResourceFlags & IORESOURCE_IO)
                    // Io Ports
                {
                    // error, but ignore it
                    bResult = TRUE;

                    // set ErrorString
                    fnBuildString(  _TEXT(__FUNCTION__),
                                    _TEXT("(): Unexpected IO Ressource!"),
                                    NULL,
                                    lDimErrorIn,
                                    sErrorOut);

                }
                else if (lResourceFlags & IORESOURCE_IO)
                        // Interrupt
                {
                    //ToDo:
                    /*if    ((pResDescriptor->Flags & CM_RESOURCE_INTERRUPT_MESSAGE) == CM_RESOURCE_INTERRUPT_MESSAGE)
                        // MessageSignaledInt (MSI)
                    {
                        // error
                        bResult = FALSE;

                        // set ErrorString
                        fnBuildString(  _TEXT(__FUNCTION__),
                                        _TEXT("(): Unexpected message signaled interrupt (MSI)!"),
                                        NULL,
                                        lDimErrorIn,
                                        sErrorOut);
                    }*/

                    // set ErrorString
                    fnBuildString(  _TEXT(__FUNCTION__),
                                    _TEXT("(): Unexpected Interrupt Ressource!"),
                                    NULL,
                                    lDimErrorIn,
                                    sErrorOut);

                }
                else if (lResourceFlags & IORESOURCE_MEM)
                        // Memory
                {
                    switch  (i)
                            // BarIndex
                    {
                        case 0:
                        case 1:
                        case 2:
                        case 3:
                        case 4:
                        case 5:
                        {
                            // store at DeviceExtension
                            pDevExtIn->uHwResKm.uBar[i].uPhyAdr.QuadPart    = pci_resource_start(   pDevExtIn->pPciDev, i);
                            pDevExtIn->uHwResKm.uBar[i].lSize               = pci_resource_len(     pDevExtIn->pPciDev, i);

                            break;
                        }
                        default:
                        {
                        _TCHAR sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

                            // error
                            bResult = FALSE;

                            // set ErrorDetail
                            _RtlStringCchPrintf(sErrorDetail,
                                                _countof(sErrorDetail),
                                                _TEXT("(): Invalid BAR number (%u)!"),
                                                i);

                            // set ErrorString
                            fnBuildString(  _TEXT(__FUNCTION__),
                                            sErrorDetail,
                                            NULL,
                                            lDimErrorIn,
                                            sErrorOut);

                            break;
                        }
                    }
                }
                else
                {
                    // ignore all other flags
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
//  get ConfigSpaceInfo
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN fnGetInfoConfigSpace(   DEVICE_EXTENSION*       pDevExtIn,
                                const UINT32            lDimErrorIn,
                                uPNDEV_CONFIG_SPACE*    pConfigSpaceOut,
                                _TCHAR*                 sErrorOut)
{
BOOLEAN                 bResult     = FALSE;
NTSTATUS                lResultTmp  = STATUS_UNSUCCESSFUL;
BUS_INTERFACE_STANDARD  uBusInterface;

    lResultTmp = WdfFdoQueryForInterface(   pDevExtIn->hDeviceOs,
                                            &GUID_BUS_INTERFACE_STANDARD,
                                            (PINTERFACE) &uBusInterface,
                                            sizeof(BUS_INTERFACE_STANDARD),
                                            1,                                      // Version
                                            NULL);                                  // InterfaceSpecificData

    if  (lResultTmp != STATUS_SUCCESS)
        // error
    {
        // set ErrorString
        fnBuildString(  _TEXT(__FUNCTION__),
                        _TEXT("(): Error at acquiring BusInterface!"),
                        NULL,
                        lDimErrorIn,
                        sErrorOut);
    }
    else
    {
    PCI_COMMON_HEADER   uHeader;
    PCI_COMMON_CONFIG*  pPciConfig  = (PCI_COMMON_CONFIG*) &uHeader;

        // success
        bResult = TRUE;

        // PCI_COMMON_HEADER structure: used to retrieve the 64 bytes of data that precedes the device-specific data
        // PCI_COMMON_CONFIG structure: includes device specific data

        // read first part of the header to get the StatusRegister and the CapabilitiesPtr
        //  - CapabilitiesPtr is an offset from the beginning of the header to a linked list of capabilities
        uBusInterface.GetBusData(   uBusInterface.Context,
                                    PCI_WHICHSPACE_CONFIG,
                                    pPciConfig,
                                    0,
                                    sizeof(PCI_COMMON_HEADER));

        // save ConfigSpace values
        //  - type 0 is the standard type
        pConfigSpaceOut->lIntLine   = pPciConfig->u.type0.InterruptLine;
        pConfigSpaceOut->lIntPin    = pPciConfig->u.type0.InterruptPin;

        if  (pPciConfig->Status & PCI_STATUS_CAPABILITIES_LIST)
            // CapabilityList present
        {
        PCI_EXPRESS_CAPABILITY  uPciExpressCapability;
        UINT32                  lOffsetCapability   = 0;
        UINT32                  lIntLine            = 0;
        UINT32                  lIntPin             = 0;
        BOOLEAN                 bDebug              = FALSE;

            // device supports capability lists -> find the capabilities

            // get offset to CapabilitiesPtr
            //  - position of the CapabilitiesPtr in the header depends on whether this is a BridgeTypeDevice
            {
                if  ((pPciConfig->HeaderType & (~PCI_MULTIFUNCTION)) == PCI_BRIDGE_TYPE)
                    // PciConfigType = PCI to PCI Bridge
                {
                    lOffsetCapability = pPciConfig->u.type1.CapabilitiesPtr;
                }
                else if ((pPciConfig->HeaderType & (~PCI_MULTIFUNCTION)) == PCI_CARDBUS_BRIDGE_TYPE)
                        // PciConfigType = PCI to CARDBUS Bridge
                {
                    lOffsetCapability = pPciConfig->u.type2.CapabilitiesPtr;
                }
                else
                {
                    lOffsetCapability = pPciConfig->u.type0.CapabilitiesPtr;
                }
            }

            // loop through the capabilities in search of the desired capability
            //  - the list is NULL-terminated, so the last offset will always be zero

            while   (lOffsetCapability)
                    // not end of list
            {
                // read the header of the capability at this offset
                uBusInterface.GetBusData(   uBusInterface.Context,
                                            PCI_WHICHSPACE_CONFIG,
                                            &uPciExpressCapability,
                                            lOffsetCapability,
                                            sizeof(PCI_CAPABILITIES_HEADER));

                if  (uPciExpressCapability.Header.CapabilityID == PCI_CAPABILITY_ID_PCI_EXPRESS)
                    // PCIe capability found
                {
                    // leave loop
                    break;
                }
                else
                {
                    // look to next capability
                    lOffsetCapability = uPciExpressCapability.Header.Next;
                }
            }

            if  (lOffsetCapability != 0)
                // PCIe CapabilityList found
            {
                // extended PCIe ConfigSpace exist
                pConfigSpaceOut->bExtendedExist = TRUE;

                // read PCIe CapabilityList
                uBusInterface.GetBusData(   uBusInterface.Context,
                                            PCI_WHICHSPACE_CONFIG,
                                            &uPciExpressCapability.ExpressCapabilities,
                                            lOffsetCapability + sizeof(PCI_CAPABILITIES_HEADER),
                                            sizeof(PCI_EXPRESS_CAPABILITY) - sizeof(PCI_CAPABILITIES_HEADER));

                // save ConfigSpace values
                {
                    pConfigSpaceOut->uExtended.lLinkWidth = uPciExpressCapability.LinkStatus.LinkWidth;

                    switch  (uPciExpressCapability.DeviceControl.MaxPayloadSize)
                            // MaxPayloadSize
                    {
                        case MaxPayload128Bytes:    {pConfigSpaceOut->uExtended.lMaxPayloadSize = 128;  break;}
                        case MaxPayload256Bytes:    {pConfigSpaceOut->uExtended.lMaxPayloadSize = 256;  break;}
                        case MaxPayload512Bytes:    {pConfigSpaceOut->uExtended.lMaxPayloadSize = 512;  break;}
                        case MaxPayload1024Bytes:   {pConfigSpaceOut->uExtended.lMaxPayloadSize = 1024; break;}
                        case MaxPayload2048Bytes:   {pConfigSpaceOut->uExtended.lMaxPayloadSize = 2048; break;}
                        case MaxPayload4096Bytes:   {pConfigSpaceOut->uExtended.lMaxPayloadSize = 4096; break;}
                        default:
                        {
                            break;
                        }
                    }

                    if  (uPciExpressCapability.DeviceStatus.CorrectableErrorDetected)   {pConfigSpaceOut->uExtended.bCorrectableErrorDetected   = TRUE;}
                    if  (uPciExpressCapability.DeviceStatus.NonFatalErrorDetected)      {pConfigSpaceOut->uExtended.bNonFatalErrorDetected      = TRUE;}
                    if  (uPciExpressCapability.DeviceStatus.FatalErrorDetected)         {pConfigSpaceOut->uExtended.bFatalErrorDetected         = TRUE;}
                    if  (uPciExpressCapability.DeviceStatus.UnsupportedRequestDetected) {pConfigSpaceOut->uExtended.bUnsupportedRequestDetected = TRUE;}
                    if  (uPciExpressCapability.DeviceStatus.AuxPowerDetected)           {pConfigSpaceOut->uExtended.bAuxPowerDetected           = TRUE;}
                    if  (uPciExpressCapability.DeviceStatus.TransactionsPending)        {pConfigSpaceOut->uExtended.bTransactionsPending        = TRUE;}
                    
                    // Flr supported by this Device
                    if  (uPciExpressCapability.DeviceCapabilities.Rsvd2 & PNDEV_UINT32_SET_BIT_0)	{pDevExtIn->bFlrSupport = TRUE;} 
                }

                // ack ErrorBits
                //  - write DeviceStatus back to PCIe CapabilityList
                uBusInterface.SetBusData(   uBusInterface.Context,
                                            PCI_WHICHSPACE_CONFIG,
                                            &uPciExpressCapability.DeviceStatus.AsUSHORT,
                                            lOffsetCapability + FIELD_OFFSET(PCI_EXPRESS_CAPABILITY, DeviceStatus),
                                            sizeof(USHORT));

                // debug purpose
                uBusInterface.GetBusData(   uBusInterface.Context,
                                            PCI_WHICHSPACE_CONFIG,
                                            &uPciExpressCapability.ExpressCapabilities,
                                            lOffsetCapability + sizeof(PCI_CAPABILITIES_HEADER),
                                            sizeof(PCI_EXPRESS_CAPABILITY) - sizeof(PCI_CAPABILITIES_HEADER));

                // debug purpose
                bDebug = FALSE;
/*
                DevExt->busInterface.GetBusData(    DevExt->busInterface.Context,
                                                    PCI_WHICHSPACE_CONFIG,
                                                    &uPciExpressCapability.ExpressCapabilities,
                                                    capabilityOffset + sizeof(PCI_CAPABILITIES_HEADER),
                                                    sizeof(PCI_EXPRESS_CAPABILITY) - sizeof(PCI_CAPABILITIES_HEADER));

                uPciExpressCapability.DeviceControl.NoSnoopEnable = 1;

                DevExt->busInterface.SetBusData(    DevExt->busInterface.Context,
                                                    PCI_WHICHSPACE_CONFIG,
                                                    &uPciExpressCapability.DeviceControl.AsUSHORT,
                                                    capabilityOffset + FIELD_OFFSET(PCI_EXPRESS_CAPABILITY, DeviceControl),
                                                    sizeof(USHORT));
*/
            }
        }
    }

    return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
BOOLEAN fnGetInfoConfigSpace(   DEVICE_EXTENSION*       pDevExtIn,
                                const UINT32            lDimErrorIn,
                                uPNDEV_CONFIG_SPACE*    pConfigSpaceOut,
                                _TCHAR*                 sErrorOut)
{
BOOLEAN bResult     = FALSE;
BOOLEAN bResultInt  = FALSE;
int32_t lResultTmp  = ADN_ERROR;

    // get IntPin / IntLine
    {
    uint8_t lIntPin = 0;

        // Note:
        //  - 1=INTA, 2=INTB, 3=INTC, 4=INTD

        // get IntPin
        lResultTmp = adn_pci_read_config_uint8( pDevExtIn->uPciLoc.uNo.lBus,
                                                pDevExtIn->uPciLoc.uNo.lDevice,
                                                pDevExtIn->uPciLoc.uNo.lFunction,
                                                ADN_PCI_CFG_DEV_INT_PIN,
                                                &lIntPin);

        if  (lResultTmp != ADN_OK)
            // error
        {
            // set ErrorString
            fnBuildString(  _TEXT(__FUNCTION__),
                            _TEXT("(): Error at reading IntPin!"),
                            NULL,
                            lDimErrorIn,
                            sErrorOut);
        }
        else
        {
            // save ConfigSpace value
            pConfigSpaceOut->lIntPin = lIntPin;

            // get IntLine
            {
            uint8_t lIntLine = 0;

                // Note:
                //  - there is an Adonis workaround
                //      IntLine of IO-APIC is identified by ACPI driver and written to ConfigSpace

                if  (acpi_drv_install() != ADN_OK)
                    // error at installing ACPI driver
                {
                    // set ErrorString
                    fnBuildString(  _TEXT(__FUNCTION__),
                                    _TEXT("(): Error at acpi_drv_install()!"),
                                    NULL,
                                    lDimErrorIn,
                                    sErrorOut);
                }
                else
                {
                    // update IntLine at ConfigSpace
                    lResultTmp = acpi_drv_write_ioapic_irqnum_to_pci_configspace(   pDevExtIn->uPciLoc.uNo.lBus,
                                                                                    pDevExtIn->uPciLoc.uNo.lDevice,
                                                                                    pDevExtIn->uPciLoc.uNo.lFunction);

                    if  (lResultTmp != ADN_OK)
                        // error
                    {
                        // set ErrorString
                        fnBuildString(  _TEXT(__FUNCTION__),
                                        _TEXT("(): Error at acpi_drv_write_ioapic_irqnum_to_pci_configspace()!"),
                                        NULL,
                                        lDimErrorIn,
                                        sErrorOut);
                    }
                    else
                    {
                        if (acpi_drv_uninstall() != ADN_OK)
                            // error at uninstalling ACPI driver
                        {
                            // set ErrorString
                            fnBuildString(  _TEXT(__FUNCTION__),
                                            _TEXT("(): Error at acpi_drv_uninstall()!"),
                                            NULL,
                                            lDimErrorIn,
                                            sErrorOut);
                        }
                        else
                        {
                            // get IntLine
                            lResultTmp = adn_pci_read_config_uint8( pDevExtIn->uPciLoc.uNo.lBus,
                                                                    pDevExtIn->uPciLoc.uNo.lDevice,
                                                                    pDevExtIn->uPciLoc.uNo.lFunction,
                                                                    ADN_PCI_CFG_DEV_INT_LINE,
                                                                    &lIntLine);

                            if  (lResultTmp != ADN_OK)
                                // error
                            {
                                // set ErrorString
                                fnBuildString(  _TEXT(__FUNCTION__),
                                                _TEXT("(): Error at reading IntLine!"),
                                                NULL,
                                                lDimErrorIn,
                                                sErrorOut);
                            }
                            else
                            {
                                // success
                                bResultInt = TRUE;

                                // store value
                                pConfigSpaceOut->lIntLine   = lIntLine;
                                pDevExtIn->lIntLine         = lIntLine;
                            }
                        }
                    }
                }
            }
        }
    }

    if  (bResultInt)
        // success
    {
    uint16_t lStatus = 0;

        // get status
        lResultTmp = adn_pci_read_config_uint16(pDevExtIn->uPciLoc.uNo.lBus,
                                                pDevExtIn->uPciLoc.uNo.lDevice,
                                                pDevExtIn->uPciLoc.uNo.lFunction,
                                                ADN_PCI_CFG_STATUS,
                                                &lStatus);

        if  (lResultTmp != ADN_OK)
            // error
        {
            // set ErrorString
            fnBuildString(  _TEXT(__FUNCTION__),
                            _TEXT("Error at reading status!"),
                            NULL,
                            lDimErrorIn,
                            sErrorOut);
        }
        else
        {
            if  ((lStatus & PNDEV_PCI_STATUS_CAPABILITIES_LIST) == 0)
                // no CapabilityList present
            {
                // success
                bResult = TRUE;
            }
            else
            {
            uint8_t lHeaderType = 0;

                // device supports CapabilityLists -> find the capabilities

                // get HeaderType
                lResultTmp = adn_pci_read_config_uint8( pDevExtIn->uPciLoc.uNo.lBus,
                                                        pDevExtIn->uPciLoc.uNo.lDevice,
                                                        pDevExtIn->uPciLoc.uNo.lFunction,
                                                        ADN_PCI_CFG_HEADER_TYPE,
                                                        &lHeaderType);

                if  (lResultTmp != ADN_OK)
                    // error
                {
                    // set ErrorString
                    fnBuildString(  _TEXT(__FUNCTION__),
                                    _TEXT("Error at reading HeaderType!"),
                                    NULL,
                                    lDimErrorIn,
                                    sErrorOut);
                }
                else
                {
                uint8_t lOffsetCapability   = 0;
                BOOLEAN bFoundPcie          = FALSE;

                    // get offset to CapabilitiesPtr
                    //  - position of the CapabilitiesPtr in the header depends on whether this is a BridgeTypeDevice

                    if  ((lHeaderType & (~PNDEV_PCI_MULTIFUNCTION)) == PNDEV_PCI_BRIDGE_TYPE)
                        // PciConfigType = PCI to PCI Bridge
                    {
                        // Type1
                        lOffsetCapability = PNDEV_PCI_CAPABILITY_PTR_TYPE_0_1;
                    }
                    else if ((lHeaderType & (~PNDEV_PCI_MULTIFUNCTION)) == PNDEV_PCI_CARDBUS_BRIDGE_TYPE)
                            // PciConfigType = PCI to CARDBUS Bridge
                    {
                        // Type2
                        lOffsetCapability = PNDEV_PCI_CAPABILITY_PTR_TYPE_2;
                    }
                    else
                    {
                        // Type0
                        lOffsetCapability = PNDEV_PCI_CAPABILITY_PTR_TYPE_0_1;
                    }

                    // read address of first capability
                    lResultTmp = adn_pci_read_config_uint8( pDevExtIn->uPciLoc.uNo.lBus,
                                                            pDevExtIn->uPciLoc.uNo.lDevice,
                                                            pDevExtIn->uPciLoc.uNo.lFunction,
                                                            lOffsetCapability,
                                                            &lOffsetCapability);

                    if  (lResultTmp != ADN_OK)
                        // error
                    {
                        // set ErrorString
                        fnBuildString(  _TEXT(__FUNCTION__),
                                        _TEXT("Error at reading OffsetCapability!"),
                                        NULL,
                                        lDimErrorIn,
                                        sErrorOut);

                    }
                    else
                    {
                        // loop through the capabilities, search for the desired capability
                        //  - the list is NULL-terminated, so the last offset will always be zero

                        // preset
                        bResult = TRUE;

                        while   (lOffsetCapability)
                                // not end of list
                        {
                        uint8_t lCapabilityId = 0;

                            // read CapabilityId
                            lResultTmp =  adn_pci_read_config_uint8(pDevExtIn->uPciLoc.uNo.lBus,
                                                                    pDevExtIn->uPciLoc.uNo.lDevice,
                                                                    pDevExtIn->uPciLoc.uNo.lFunction,
                                                                    lOffsetCapability,
                                                                    &lCapabilityId);

                            if  (lResultTmp != ADN_OK)
                                // error
                            {
                                bResult = FALSE;

                                // set ErrorString
                                fnBuildString(  _TEXT(__FUNCTION__),
                                                _TEXT("Error at reading CapabilityId!"),
                                                NULL,
                                                lDimErrorIn,
                                                sErrorOut);

                                // leave loop
                                break;
                            }
                            else
                            {
                                if  (lCapabilityId == PNDEV_PCIE_CAPABILITY_ID)
                                    // PCIe capability found
                                {
                                    // success
                                    bFoundPcie = TRUE;

                                    // leave loop
                                    break;
                                }
                                else
                                {
                                    // get next CapabilityPtr
                                    lResultTmp = adn_pci_read_config_uint8( pDevExtIn->uPciLoc.uNo.lBus,
                                                                            pDevExtIn->uPciLoc.uNo.lDevice,
                                                                            pDevExtIn->uPciLoc.uNo.lFunction,
                                                                            lOffsetCapability + sizeof(UINT8),
                                                                            &lOffsetCapability);

                                    if  (lResultTmp != ADN_OK)
                                        // error
                                    {
                                        bResult = FALSE;

                                        // set ErrorString
                                        fnBuildString(  _TEXT(__FUNCTION__),
                                                        _TEXT("Error at reading OffsetCapability!"),
                                                        NULL,
                                                        lDimErrorIn,
                                                        sErrorOut);

                                        // leave loop
                                        break;
                                    }
                                }
                            }
                        }
                    }

                    if  (bResult)
                        // success
                    {
                        if  (bFoundPcie)
                            // PCIe CapabilityList found
                        {
                            // preset
                            bResult = FALSE;

                            // get data of PCIe Capability
                            bResult = fnGetExtendedConfigSpace( pDevExtIn,
                                                                lOffsetCapability,
                                                                lDimErrorIn,
                                                                pConfigSpaceOut,
                                                                sErrorOut);
                        }
                    }
                }
            }
        }
    }

    return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN fnGetInfoConfigSpace(   DEVICE_EXTENSION*       pDevExtIn,
                                const UINT32            lDimErrorIn,
                                uPNDEV_CONFIG_SPACE*    pConfigSpaceOut,
                                _TCHAR*                 sErrorOut)
{
BOOLEAN bResult     = FALSE;
BOOLEAN bResultInt  = FALSE;
UINT16  lResultTmp  = LINUX_ERROR;

    // get IntPin / IntLine
    {
        if  (pDevExtIn->pPciDev->pin <= 0)
            // error
        {
            // set ErrorString
            fnBuildString(  _TEXT(__FUNCTION__),
                            _TEXT("(): Error at reading IntPin!"),
                            NULL,
                            lDimErrorIn,
                            sErrorOut);
        }
        else
        {
            // save IntPin value
            pConfigSpaceOut->lIntPin = pDevExtIn->pPciDev->pin;

            // get IntLine
            {
                if  (pDevExtIn->pPciDev->irq <= 0)
                    // error
                {
                    // set ErrorString
                    fnBuildString(  _TEXT(__FUNCTION__),
                                    _TEXT("(): Error at reading IntLine!"),
                                    NULL,
                                    lDimErrorIn,
                                    sErrorOut);
                }
                else
                {
                    // success
                    bResultInt = TRUE;

                    // store value
                    pConfigSpaceOut->lIntLine   = pDevExtIn->pPciDev->irq;
                    pDevExtIn->lIntLine         = pDevExtIn->pPciDev->irq;
                }
            }
        }
    }

    if  (bResultInt)
        // success
    {
    UINT16 lStatus = 0;

        // get status
        lResultTmp = pci_read_config_word(  pDevExtIn->pPciDev,
                                            PCI_STATUS,
                                            &lStatus);

        if  (lResultTmp != LINUX_OK)
            // error
        {
            // set ErrorString
            fnBuildString(  _TEXT(__FUNCTION__),
                            _TEXT("Error at reading status!"),
                            NULL,
                            lDimErrorIn,
                            sErrorOut);
        }
        else
        {
            if  ((lStatus & PNDEV_PCI_STATUS_CAPABILITIES_LIST) == 0)
                // no CapabilityList present
            {
                // success
                bResult = TRUE;
            }
            else
            {
            UINT8 lHeaderType = 0;

                // device supports CapabilityLists -> find the capabilities

                // get HeaderType
                lResultTmp = pci_read_config_byte(  pDevExtIn->pPciDev,
                                                    PCI_HEADER_TYPE,
                                                    &lHeaderType);

                if  (lResultTmp != LINUX_OK)
                    // error
                {
                    // set ErrorString
                    fnBuildString(  _TEXT(__FUNCTION__),
                                    _TEXT("Error at reading HeaderType!"),
                                    NULL,
                                    lDimErrorIn,
                                    sErrorOut);
                }
                else
                {
                uint8_t lOffsetCapability   = 0;
                BOOLEAN bFoundPcie          = FALSE;

                    // get offset to CapabilitiesPtr
                    //  - position of the CapabilitiesPtr in the header depends on whether this is a BridgeTypeDevice

                    if  ((lHeaderType & (~PNDEV_PCI_MULTIFUNCTION)) == PNDEV_PCI_BRIDGE_TYPE)
                        // PciConfigType = PCI to PCI Bridge
                    {
                        // Type1
                        lOffsetCapability = PNDEV_PCI_CAPABILITY_PTR_TYPE_0_1;
                    }
                    else if ((lHeaderType & (~PNDEV_PCI_MULTIFUNCTION)) == PNDEV_PCI_CARDBUS_BRIDGE_TYPE)
                            // PciConfigType = PCI to CARDBUS Bridge
                    {
                        // Type2
                        lOffsetCapability = PNDEV_PCI_CAPABILITY_PTR_TYPE_2;
                    }
                    else
                    {
                        // Type0
                        lOffsetCapability = PNDEV_PCI_CAPABILITY_PTR_TYPE_0_1;
                    }

                    // read address of first capability
                    lResultTmp = pci_read_config_byte(  pDevExtIn->pPciDev,
                                                        lOffsetCapability,
                                                        &lOffsetCapability);

                    if  (lResultTmp != LINUX_OK)
                        // error
                    {
                        // set ErrorString
                        fnBuildString(  _TEXT(__FUNCTION__),
                                        _TEXT("Error at reading OffsetCapability!"),
                                        NULL,
                                        lDimErrorIn,
                                        sErrorOut);

                    }
                    else
                    {
                        // loop through the capabilities, search for the desired capability
                        //  - the list is NULL-terminated, so the last offset will always be zero

                        // preset
                        bResult = TRUE;

                        while   (lOffsetCapability)
                                // not end of list
                        {
                        uint8_t lCapabilityId = 0;

                            // read CapabilityId
                            lResultTmp =  pci_read_config_byte( pDevExtIn->pPciDev,
                                                                lOffsetCapability,
                                                                &lCapabilityId);

                            if  (lResultTmp != LINUX_OK)
                                // error
                            {
                                bResult = FALSE;

                                // set ErrorString
                                fnBuildString(  _TEXT(__FUNCTION__),
                                                _TEXT("Error at reading CapabilityId!"),
                                                NULL,
                                                lDimErrorIn,
                                                sErrorOut);

                                // leave loop
                                break;
                            }
                            else
                            {
                                if  (lCapabilityId == PNDEV_PCIE_CAPABILITY_ID)
                                    // PCIe capability found
                                {
                                    // success
                                    bFoundPcie = TRUE;

                                    // leave loop
                                    break;
                                }
                                else
                                {
                                    // get next CapabilityPtr
                                    lResultTmp = pci_read_config_byte(  pDevExtIn->pPciDev,
                                                                        lOffsetCapability + sizeof(UINT8),
                                                                        &lOffsetCapability);

                                    if  (lResultTmp != LINUX_OK)
                                        // error
                                    {
                                        bResult = FALSE;

                                        // set ErrorString
                                        fnBuildString(  _TEXT(__FUNCTION__),
                                                        _TEXT("Error at reading OffsetCapability!"),
                                                        NULL,
                                                        lDimErrorIn,
                                                        sErrorOut);

                                        // leave loop
                                        break;
                                    }
                                }
                            }
                        }
                    }

                    if  (bResult)
                        // success
                    {
                        if  (bFoundPcie)
                            // PCIe CapabilityList found
                        {
                            // preset
                            bResult = FALSE;

                            // get data of PCIe Capability
                            bResult = fnGetExtendedConfigSpace( pDevExtIn,
                                                                lOffsetCapability,
                                                                lDimErrorIn,
                                                                pConfigSpaceOut,
                                                                sErrorOut);
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

//************************************************************************
//  D e s c r i p t i o n :
//
//  get BarValues of ConfigSpace
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
BOOLEAN fnGetBarConfigSpace(DEVICE_EXTENSION*   pDevExtIn,
                            const UINT32        lBarIndexIn,
                            const BOOLEAN       bGetSizeIn,
                            const UINT32        lDimErrorIn,
                            UINT32*             pPhyAdrOut,
                            UINT32*             pSizeOut,
                            _TCHAR*             sErrorOut)
{
BOOLEAN     bResult     = FALSE;
BOOLEAN     bResultAdr  = FALSE;
uint16_t    lBarReg     = 0;

    // preset OutputParameter
    *pPhyAdrOut = 0;
    *pSizeOut   = 0;

    // preset
    bResultAdr = TRUE;

    switch  (lBarIndexIn)
            // BarIndex
    {
        case 0: {lBarReg = ADN_PCI_CFG_BASE_ADDRESS_0;  break;}
        case 1: {lBarReg = ADN_PCI_CFG_BASE_ADDRESS_1;  break;}
        case 2: {lBarReg = ADN_PCI_CFG_BASE_ADDRESS_2;  break;}
        case 3: {lBarReg = ADN_PCI_CFG_BASE_ADDRESS_3;  break;}
        case 4: {lBarReg = ADN_PCI_CFG_BASE_ADDRESS_4;  break;}
        case 5: {lBarReg = ADN_PCI_CFG_BASE_ADDRESS_5;  break;}
        default:
        {
        _TCHAR sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

            // error
            bResultAdr = FALSE;

            // set ErrorDetail
            _RtlStringCchPrintf(sErrorDetail,
                                _countof(sErrorDetail),
                                _TEXT("(): Invalid BAR number (%u)!"),
                                lBarIndexIn);

            // set ErrorString
            fnBuildString(  _TEXT(__FUNCTION__),
                            sErrorDetail,
                            NULL,
                            lDimErrorIn,
                            sErrorOut);

            break;
        }
    }

    if  (bResultAdr)
        // success
    {
    int32_t     lResultTmp  = ADN_ERROR;
    uint32_t    lPhyAdr     = 0;

        // get PhyAdr
        lResultTmp = adn_pci_read_config_uint32(pDevExtIn->uPciLoc.uNo.lBus,
                                                pDevExtIn->uPciLoc.uNo.lDevice,
                                                pDevExtIn->uPciLoc.uNo.lFunction,
                                                lBarReg,
                                                &lPhyAdr);

        if  (lResultTmp != ADN_OK)
            // error
        {
            // set ErrorString
            fnBuildString(  _TEXT(__FUNCTION__),
                            _TEXT("(): Error at reading PhyAdr!"),
                            NULL,
                            lDimErrorIn,
                            sErrorOut);
        }
        else
        {
            if  (!bGetSizeIn)
                // getting size not desired
                //  - writing to ConfigSpace not always allowed!
            {
                // success
                bResult = TRUE;
            }
            else
            {
            BOOLEAN 	bResultSize = FALSE;
            uint32_t 	lBarSize 	= 0;
            
                // Note:
                //  - PhyAdr.Bit0:      0=MemoryMapped
                //                      1=I/O
                //  - PhyAdr.Bit2/1:    00=32Bit anywhere
                //                      01=32Bit <1MB
                //                      10=64Bit
                //                      11=reserved
                //  - PhyAdr.Bit3:      0=NonPrefetchable, 1=prefetchable

                if  ((lPhyAdr & PNDEV_UINT32_SET_BIT_0) == PNDEV_UINT32_SET_BIT_0)
                    // I/O BaseAdr
                {
                /*  // set ErrorString
                    fnBuildString(  _TEXT(__FUNCTION__),
                                    _TEXT("(): BarBaseAdr is an I/O address!"),
                                    NULL,
                                    lDimErrorIn,
                                    sErrorOut);*/

                    //  ignore this case
                    // success
                    bResult = TRUE;
                }
              /*  else if ((lPhyAdr & (PNDEV_UINT32_SET_BIT_2 + PNDEV_UINT32_SET_BIT_1)) == PNDEV_UINT32_SET_BIT_2)
                        // 64Bit BaseAdr
                {
                    // set ErrorString
                    fnBuildString(  _TEXT(__FUNCTION__),
                                    _TEXT("(): BarBaseAdr is a 64Bit address!"),
                                    NULL,
                                    lDimErrorIn,
                                    sErrorOut);
                }*/
                else if ((lPhyAdr & (PNDEV_UINT32_SET_BIT_2 + PNDEV_UINT32_SET_BIT_1)) == (PNDEV_UINT32_SET_BIT_2 + PNDEV_UINT32_SET_BIT_1))
                        // reserved type
                {
                    // set ErrorString
                    fnBuildString(  _TEXT(__FUNCTION__),
                                    _TEXT("(): BarBaseAdr uses reserved type!"),
                                    NULL,
                                    lDimErrorIn,
                                    sErrorOut);
                }
                else
                {
                
                	if  (lPhyAdr != 0)
                		// valid PhyAdr
                	{
						// preset
						lResultTmp = ADN_ERROR;
	
						// write 0xFFFFffff to BarReg for getting size
						lResultTmp = adn_pci_write_config_uint32(   pDevExtIn->uPciLoc.uNo.lBus,
																	pDevExtIn->uPciLoc.uNo.lDevice,
																	pDevExtIn->uPciLoc.uNo.lFunction,
																	lBarReg,
																	0xFFFFffff);
	
						if  (lResultTmp != ADN_OK)
							// error
						{
							// set ErrorString
							fnBuildString(  _TEXT(__FUNCTION__),
											_TEXT("(): Error at writing 0xFFFFffff to BarReg!"),
											NULL,
											lDimErrorIn,
											sErrorOut);
						}
						else
						{
							// preset
							lResultTmp = ADN_ERROR;
	
							// get BarSize
							lResultTmp = adn_pci_read_config_uint32(pDevExtIn->uPciLoc.uNo.lBus,
																	pDevExtIn->uPciLoc.uNo.lDevice,
																	pDevExtIn->uPciLoc.uNo.lFunction,
																	lBarReg,
																	&lBarSize);
	
							if  (lResultTmp != ADN_OK)
								// error
							{
								// set ErrorString
								fnBuildString(  _TEXT(__FUNCTION__),
												_TEXT("(): Error at reading BarSize!"),
												NULL,
												lDimErrorIn,
												sErrorOut);
							}
							else
							{
								// success
								bResultSize = TRUE;
								
								// compute real BarSize
								{
								UINT32 i = 0;
	
									// Note:
									//  - size can be computed by following algorithm:
									//      search for first bit <> 0 beginning by bit4
									//      -> size = this bit
	
									for (i = 4; i <= 31; i++)
										// Bit4...Bit32
									{
										if  ((lBarSize & (1<<i)) == (1<<i))
											// first bit <> 0 found
										{
											lBarSize = (1<<i);
	
											// leave loop
											break;
										}
									}
								}
							}
						}
                	}
                	else
                	{
                		bResultSize = TRUE;
                	}
                }
                
                if	(bResultSize)
                	// success
                {
                    // preset
                    lResultTmp = ADN_ERROR;

                    // restore BarReg
                    lResultTmp = adn_pci_write_config_uint32(   pDevExtIn->uPciLoc.uNo.lBus,
                                                                pDevExtIn->uPciLoc.uNo.lDevice,
                                                                pDevExtIn->uPciLoc.uNo.lFunction,
                                                                lBarReg,
                                                                lPhyAdr);

                    if  (lResultTmp != ADN_OK)
                        // error
                    {
                        // set ErrorString
                        fnBuildString(  _TEXT(__FUNCTION__),
                                        _TEXT("(): Error at restoring BarReg!"),
                                        NULL,
                                        lDimErrorIn,
                                        sErrorOut);
                    }
                    else
                    {
                    uint32_t lPhyAdrTmp = 0;

                        // preset
                        lResultTmp = ADN_ERROR;

                        // get PhyAdr
                        lResultTmp = adn_pci_read_config_uint32(pDevExtIn->uPciLoc.uNo.lBus,
                                                                pDevExtIn->uPciLoc.uNo.lDevice,
                                                                pDevExtIn->uPciLoc.uNo.lFunction,
                                                                lBarReg,
                                                                &lPhyAdrTmp);

                        if  (lResultTmp != ADN_OK)
                            // error
                        {
                            // set ErrorString
                            fnBuildString(  _TEXT(__FUNCTION__),
                                            _TEXT("(): Error at reading PhyAdr!"),
                                            NULL,
                                            lDimErrorIn,
                                            sErrorOut);
                        }
                        else
                        {
                            if  (lPhyAdrTmp != lPhyAdr)
                                // error at verifying
                            {
                                // set ErrorString
                                fnBuildString(  _TEXT(__FUNCTION__),
                                                _TEXT("(): Error at verifying PhyAdr!"),
                                                NULL,
                                                lDimErrorIn,
                                                sErrorOut);
                            }
                            else
                            {
                                // success
                                bResult = TRUE;

                                // return
                                *pSizeOut = lBarSize;
                            }
                        }
                    }
                }
            }

            if  (bResult)
                // success
            {
                // return
                *pPhyAdrOut = ADN_PCI_CALC_PHYS_ADDR(lPhyAdr & 0xFFFFfff0);     // Bit3:0 must be set to 0
            }
        }
    }

    return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  get data of ExtendedConfigSpace from PCIe
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
BOOLEAN fnGetExtendedConfigSpace(   DEVICE_EXTENSION*       pDevExtIn,
                                    const UINT32            lOffsetCapabilityIn,
                                    const UINT32            lDimErrorIn,
                                    uPNDEV_CONFIG_SPACE*    pConfigSpaceOut,
                                    _TCHAR*                 sErrorOut)
{
BOOLEAN bResult = FALSE;

    if (lOffsetCapabilityIn == 0)
        // invalid CapabilityPtr
    {
        // set ErrorString
        fnBuildString(  _TEXT(__FUNCTION__),
                        _TEXT("Invalid OffsetCapability!"),
                        NULL,
                        lDimErrorIn,
                        sErrorOut);
    }
    else
    {
    uint16_t    lLinkStatus             = 0;
    uint16_t    lDeviceControl          = 0;
    uint16_t    lDeviceStatus           = 0;
    uint32_t    lDeviceControlStatus    = 0;
    int32_t     lResultTmp              = ADN_ERROR;
    int32_t     lResultTmp1             = ADN_ERROR;
    int32_t     lResultTmp2             = ADN_ERROR;
    int32_t     lResultTmp3             = ADN_ERROR;
    int32_t     lResultTmp4             = ADN_ERROR;

        // get LinkStatus
        lResultTmp1 = adn_pci_read_config_uint16(   pDevExtIn->uPciLoc.uNo.lBus,
                                                    pDevExtIn->uPciLoc.uNo.lDevice,
                                                    pDevExtIn->uPciLoc.uNo.lFunction,
                                                    lOffsetCapabilityIn + PNDEV_PCIE_CAPABILITY_LINK_STATUS_OFFSET,     // offset
                                                    &lLinkStatus);

        // get DeviceControl
        lResultTmp2 = adn_pci_read_config_uint16(   pDevExtIn->uPciLoc.uNo.lBus,
                                                    pDevExtIn->uPciLoc.uNo.lDevice,
                                                    pDevExtIn->uPciLoc.uNo.lFunction,
                                                    lOffsetCapabilityIn + PNDEV_PCIE_CAPABILITY_DEVICE_CONTROL_OFFSET,  // offset
                                                    &lDeviceControl);

        // get DeviceStatus
        lResultTmp3 = adn_pci_read_config_uint16(   pDevExtIn->uPciLoc.uNo.lBus,
                                                    pDevExtIn->uPciLoc.uNo.lDevice,
                                                    pDevExtIn->uPciLoc.uNo.lFunction,
                                                    lOffsetCapabilityIn + PNDEV_PCIE_CAPABILITY_DEVICE_STATUS_OFFSET,   // offset
                                                    &lDeviceStatus);

        // get DeviceControl + DeviceStatus
        //  - required for ack of ErrorBits
        lResultTmp4 = adn_pci_read_config_uint32(   pDevExtIn->uPciLoc.uNo.lBus,
                                                    pDevExtIn->uPciLoc.uNo.lDevice,
                                                    pDevExtIn->uPciLoc.uNo.lFunction,
                                                    lOffsetCapabilityIn + PNDEV_PCIE_CAPABILITY_DEVICE_CONTROL_OFFSET,  // offset
                                                    &lDeviceControlStatus);

        if  (   (lResultTmp1 != ADN_OK)
            ||  (lResultTmp2 != ADN_OK)
            ||  (lResultTmp3 != ADN_OK)
            ||  (lResultTmp4 != ADN_OK))
            // error
        {
            // set ErrorString
            fnBuildString(  _TEXT(__FUNCTION__),
                            _TEXT("Error at reading data of PCIe Capability!"),
                            NULL,
                            lDimErrorIn,
                            sErrorOut);
        }
        else
        {
        UINT32  lMaxPayLoad = 0;
        UINT32  lLinkWidth  = 0;

            // save LinkWidth
            lLinkWidth= (lLinkStatus & PNDEV_PCIE_CAPABILITY_MASK_LINK_WIDTH) >> 4;

            switch  (lLinkWidth)
                    // LinkWidth
            {
                case 0x1:   {pConfigSpaceOut->uExtended.lLinkWidth = 1;     break;}
                case 0x2:   {pConfigSpaceOut->uExtended.lLinkWidth = 2;     break;}
                case 0x4:   {pConfigSpaceOut->uExtended.lLinkWidth = 4;     break;}
                case 0x8:   {pConfigSpaceOut->uExtended.lLinkWidth = 8;     break;}
                case 0x12:  {pConfigSpaceOut->uExtended.lLinkWidth = 12;    break;}
                case 0x16:  {pConfigSpaceOut->uExtended.lLinkWidth = 16;    break;}
                case 0x32:  {pConfigSpaceOut->uExtended.lLinkWidth = 32;    break;}
                default:
                {
                    // set ErrorString
                    fnBuildString(  _TEXT(__FUNCTION__),
                                    _TEXT("Invalid LinkWidth!"),
                                    NULL,
                                    lDimErrorIn,
                                    sErrorOut);

                    break;
                }
            }

            // save MaxPayloadSize
            lMaxPayLoad = (lDeviceControl & PNDEV_PCIE_CAPABILITY_MASK_PAY_LOAD_SIZE) >> 5;

            switch  (lMaxPayLoad)
                    // MaxPayloadSize
            {
                case 0: {pConfigSpaceOut->uExtended.lMaxPayloadSize = 128;  break;}
                case 1: {pConfigSpaceOut->uExtended.lMaxPayloadSize = 256;  break;}
                case 2: {pConfigSpaceOut->uExtended.lMaxPayloadSize = 512;  break;}
                case 3: {pConfigSpaceOut->uExtended.lMaxPayloadSize = 1024; break;}
                case 4: {pConfigSpaceOut->uExtended.lMaxPayloadSize = 2048; break;}
                case 5: {pConfigSpaceOut->uExtended.lMaxPayloadSize = 4096; break;}
                default:
                {
                    // set ErrorString
                    fnBuildString(  _TEXT(__FUNCTION__),
                                    _TEXT("Invalid MaxPayLoadSize!"),
                                    NULL,
                                    lDimErrorIn,
                                    sErrorOut);

                    break;
                }
            }

            if  (lDeviceStatus & PNDEV_UINT32_SET_BIT_0)    {pConfigSpaceOut->uExtended.bCorrectableErrorDetected   = TRUE;}
            if  (lDeviceStatus & PNDEV_UINT32_SET_BIT_1)    {pConfigSpaceOut->uExtended.bNonFatalErrorDetected      = TRUE;}
            if  (lDeviceStatus & PNDEV_UINT32_SET_BIT_2)    {pConfigSpaceOut->uExtended.bFatalErrorDetected         = TRUE;}
            if  (lDeviceStatus & PNDEV_UINT32_SET_BIT_3)    {pConfigSpaceOut->uExtended.bUnsupportedRequestDetected = TRUE;}
            if  (lDeviceStatus & PNDEV_UINT32_SET_BIT_4)    {pConfigSpaceOut->uExtended.bAuxPowerDetected           = TRUE;}
            if  (lDeviceStatus & PNDEV_UINT32_SET_BIT_5)    {pConfigSpaceOut->uExtended.bTransactionsPending        = TRUE;}
        }

        // ack ErrorBits
        //  - write DeviceStatus back to PCIe CapabilityList
        lResultTmp = adn_pci_write_config_uint32(   pDevExtIn->uPciLoc.uNo.lBus,
                                                    pDevExtIn->uPciLoc.uNo.lDevice,
                                                    pDevExtIn->uPciLoc.uNo.lFunction,
                                                    lOffsetCapabilityIn + PNDEV_PCIE_CAPABILITY_DEVICE_CONTROL_OFFSET,
                                                    lDeviceControlStatus);

        if  (lResultTmp != ADN_OK)
            // error
        {
            // set ErrorString
            fnBuildString(  _TEXT(__FUNCTION__),
                            _TEXT("Error at ack of ErrorBits!"),
                            NULL,
                            lDimErrorIn,
                            sErrorOut);
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
BOOLEAN fnGetExtendedConfigSpace(   DEVICE_EXTENSION*       pDevExtIn,
                                    const UINT32            lOffsetCapabilityIn,
                                    const UINT32            lDimErrorIn,
                                    uPNDEV_CONFIG_SPACE*    pConfigSpaceOut,
                                    _TCHAR*                 sErrorOut)
{
BOOLEAN bResult = FALSE;

    if (lOffsetCapabilityIn == 0)
        // invalid CapabilityPtr
    {
        // set ErrorString
        fnBuildString(  _TEXT(__FUNCTION__),
                        _TEXT("Invalid OffsetCapability!"),
                        NULL,
                        lDimErrorIn,
                        sErrorOut);
    }
    else
    {
    UINT16  lLinkStatus             = 0;
    UINT16  lDeviceControl          = 0;
    UINT16  lDeviceStatus           = 0;
    UINT32  lDeviceControlStatus    = 0;
    UINT32  lResultTmp              = LINUX_ERROR;
    UINT32  lResultTmp1             = LINUX_ERROR;
    UINT32  lResultTmp2             = LINUX_ERROR;
    UINT32  lResultTmp3             = LINUX_ERROR;
    UINT32  lResultTmp4             = LINUX_ERROR;

        // get LinkStatus
        lResultTmp1 = pci_read_config_word( pDevExtIn->pPciDev,
                                            lOffsetCapabilityIn + PNDEV_PCIE_CAPABILITY_LINK_STATUS_OFFSET,     // offset
                                            &lLinkStatus);

        // get DeviceControl
        lResultTmp2 = pci_read_config_word( pDevExtIn->pPciDev,
                                            lOffsetCapabilityIn + PNDEV_PCIE_CAPABILITY_DEVICE_CONTROL_OFFSET,  // offset
                                            &lDeviceControl);

        // get DeviceStatus
        lResultTmp3 = pci_read_config_word( pDevExtIn->pPciDev,
                                            lOffsetCapabilityIn + PNDEV_PCIE_CAPABILITY_DEVICE_STATUS_OFFSET,   // offset
                                            &lDeviceStatus);

        // get DeviceControl + DeviceStatus
        //  - required for ack of ErrorBits
        lResultTmp4 = pci_read_config_dword(pDevExtIn->pPciDev,
                                            lOffsetCapabilityIn + PNDEV_PCIE_CAPABILITY_DEVICE_CONTROL_OFFSET,  // offset
                                            &lDeviceControlStatus);

        if  (   (lResultTmp1 != LINUX_OK)
            ||  (lResultTmp2 != LINUX_OK)
            ||  (lResultTmp3 != LINUX_OK)
            ||  (lResultTmp4 != LINUX_OK))
            // error
        {
            // set ErrorString
            fnBuildString(  _TEXT(__FUNCTION__),
                            _TEXT("Error at reading data of PCIe Capability!"),
                            NULL,
                            lDimErrorIn,
                            sErrorOut);
        }
        else
        {
        UINT32  lMaxPayLoad = 0;
        UINT32  lLinkWidth  = 0;

            // save LinkWidth
            lLinkWidth= (lLinkStatus & PNDEV_PCIE_CAPABILITY_MASK_LINK_WIDTH) >> 4;

            switch  (lLinkWidth)
                    // LinkWidth
            {
                case 0x1:   {pConfigSpaceOut->uExtended.lLinkWidth = 1;     break;}
                case 0x2:   {pConfigSpaceOut->uExtended.lLinkWidth = 2;     break;}
                case 0x4:   {pConfigSpaceOut->uExtended.lLinkWidth = 4;     break;}
                case 0x8:   {pConfigSpaceOut->uExtended.lLinkWidth = 8;     break;}
                case 0x12:  {pConfigSpaceOut->uExtended.lLinkWidth = 12;    break;}
                case 0x16:  {pConfigSpaceOut->uExtended.lLinkWidth = 16;    break;}
                case 0x32:  {pConfigSpaceOut->uExtended.lLinkWidth = 32;    break;}
                default:
                {
                    // set ErrorString
                    fnBuildString(  _TEXT(__FUNCTION__),
                                    _TEXT("Invalid LinkWidth!"),
                                    NULL,
                                    lDimErrorIn,
                                    sErrorOut);

                    break;
                }
            }

            // save MaxPayloadSize
            lMaxPayLoad = (lDeviceControl & PNDEV_PCIE_CAPABILITY_MASK_PAY_LOAD_SIZE) >> 5;

            switch  (lMaxPayLoad)
                    // MaxPayloadSize
            {
                case 0: {pConfigSpaceOut->uExtended.lMaxPayloadSize = 128;  break;}
                case 1: {pConfigSpaceOut->uExtended.lMaxPayloadSize = 256;  break;}
                case 2: {pConfigSpaceOut->uExtended.lMaxPayloadSize = 512;  break;}
                case 3: {pConfigSpaceOut->uExtended.lMaxPayloadSize = 1024; break;}
                case 4: {pConfigSpaceOut->uExtended.lMaxPayloadSize = 2048; break;}
                case 5: {pConfigSpaceOut->uExtended.lMaxPayloadSize = 4096; break;}
                default:
                {
                    // set ErrorString
                    fnBuildString(  _TEXT(__FUNCTION__),
                                    _TEXT("Invalid MaxPayLoadSize!"),
                                    NULL,
                                    lDimErrorIn,
                                    sErrorOut);

                    break;
                }
            }

            if  (lDeviceStatus & PNDEV_UINT32_SET_BIT_0)    {pConfigSpaceOut->uExtended.bCorrectableErrorDetected   = TRUE;}
            if  (lDeviceStatus & PNDEV_UINT32_SET_BIT_1)    {pConfigSpaceOut->uExtended.bNonFatalErrorDetected      = TRUE;}
            if  (lDeviceStatus & PNDEV_UINT32_SET_BIT_2)    {pConfigSpaceOut->uExtended.bFatalErrorDetected         = TRUE;}
            if  (lDeviceStatus & PNDEV_UINT32_SET_BIT_3)    {pConfigSpaceOut->uExtended.bUnsupportedRequestDetected = TRUE;}
            if  (lDeviceStatus & PNDEV_UINT32_SET_BIT_4)    {pConfigSpaceOut->uExtended.bAuxPowerDetected           = TRUE;}
            if  (lDeviceStatus & PNDEV_UINT32_SET_BIT_5)    {pConfigSpaceOut->uExtended.bTransactionsPending        = TRUE;}
        }

        // ack ErrorBits
        //  - write DeviceStatus back to PCIe CapabilityList
        lResultTmp = pci_write_config_dword(pDevExtIn->pPciDev,
                                            lOffsetCapabilityIn + PNDEV_PCIE_CAPABILITY_DEVICE_CONTROL_OFFSET,
                                            lDeviceControlStatus);

        if  (lResultTmp != LINUX_OK)
            // error
        {
            // set ErrorString
            fnBuildString(  _TEXT(__FUNCTION__),
                            _TEXT("Error at ack of ErrorBits!"),
                            NULL,
                            lDimErrorIn,
                            sErrorOut);
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
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  prepare ConfigSpace for HwReset
//
//  Note:
//  - a PCI ConfigSpace consists of maximal 256 Bytes
//  - Soc1: ConfigSpace consists of 64 Bytes + PowerManagement capability (readonly)
//          restoring more than 64 Bytes creates a SystemError!
//          -> store only 64 Bytes
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN fnPrepareConfigSpaceHwReset(DEVICE_EXTENSION*   pDevExtIn,
                                    const UINT32        lDimErrorIn,
                                    UINT32              lArrayConfigSpaceOut[],     // array=ptr
                                    _TCHAR*             sErrorOut)
{
BOOLEAN                 bResult     = FALSE;
NTSTATUS                lResultTmp1 = STATUS_UNSUCCESSFUL;
BUS_INTERFACE_STANDARD  uBusInterface;

    // acquire BusInterface
    lResultTmp1 = WdfFdoQueryForInterface(  pDevExtIn->hDeviceOs,
                                            &GUID_BUS_INTERFACE_STANDARD,
                                            (PINTERFACE) &uBusInterface,
                                            sizeof(BUS_INTERFACE_STANDARD),
                                            1,                                      // Version
                                            NULL);                                  // InterfaceSpecificData

    if  (lResultTmp1 != STATUS_SUCCESS)
        // error
    {
        // set ErrorString
        fnBuildString(  _TEXT(__FUNCTION__),
                        _TEXT("(): Error at acquiring BusInterface!"),
                        NULL,
                        lDimErrorIn,
                        sErrorOut);
    }
    else
    {
    ULONG lResultTmp2 = 0;

        // read complete ConfigSpace
        lResultTmp2 = uBusInterface.GetBusData( uBusInterface.Context,
                                                PCI_WHICHSPACE_CONFIG,
                                                lArrayConfigSpaceOut,
                                                0,                              // offset at ConfigSpace
                                                PNDEV_PCI_CONFIG_SPACE__SIZE);

        if  (lResultTmp2 != PNDEV_PCI_CONFIG_SPACE__SIZE)
            // error
        {
            // set ErrorString
            fnBuildString(  _TEXT(__FUNCTION__),
                            _TEXT("(): Error at reading ConfigSpace!"),
                            NULL,
                            lDimErrorIn,
                            sErrorOut);
        }
        else
        {
        UINT16 lCommand = 0;

            // get command of ConfigSpace
            lResultTmp2 = uBusInterface.GetBusData( uBusInterface.Context,
                                                    PCI_WHICHSPACE_CONFIG,
                                                    &lCommand,
                                                    PNDEV_PCI_CONFIG_SPACE__COMMAND_OFFSET,     // offset at ConfigSpace
                                                    PNDEV_PCI_CONFIG_SPACE__COMMAND_SIZE);

            if  (lResultTmp2 != PNDEV_PCI_CONFIG_SPACE__COMMAND_SIZE)
                // error
            {
                // set ErrorString
                fnBuildString(  _TEXT(__FUNCTION__),
                                _TEXT("(): Error at reading command of ConfigSpace!"),
                                NULL,
                                lDimErrorIn,
                                sErrorOut);
            }
            else
            {
                // disable BusMaster
                lCommand &= PNDEV_UINT32_CLR_BIT_2;
                
                if	(pDevExtIn->bFlrSupport)
                	// Flr is supported
                {
                	// disable MemorySpace
                	lCommand &= PNDEV_UINT32_CLR_BIT_1;
                }

                // update command at ConfigSpace
                lResultTmp2 = uBusInterface.SetBusData( uBusInterface.Context,
                                                        PCI_WHICHSPACE_CONFIG,
                                                        &lCommand,
                                                        PNDEV_PCI_CONFIG_SPACE__COMMAND_OFFSET,     // offset into ConfigSpace
                                                        PNDEV_PCI_CONFIG_SPACE__COMMAND_SIZE);

                if  (lResultTmp2 != PNDEV_PCI_CONFIG_SPACE__COMMAND_SIZE)
                    // error
                {
                    // set ErrorString
                    fnBuildString(  _TEXT(__FUNCTION__),
                                    _TEXT("(): Error at writing Command at ConfigSpace!"),
                                    NULL,
                                    lDimErrorIn,
                                    sErrorOut);
                }
                else
                {
                    // success
                    bResult = TRUE;
                }
            }
        }
    }

    return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
BOOLEAN fnPrepareConfigSpaceHwReset(DEVICE_EXTENSION*   pDevExtIn,
                                    const UINT32        lDimErrorIn,
                                    UINT32              lArrayConfigSpaceOut[],     // array=ptr
                                    _TCHAR*             sErrorOut)
{
BOOLEAN bResult     = FALSE;
BOOLEAN bResultTmp  = FALSE;
int32_t lResultTmp  = ADN_ERROR;
UINT32  i = 0;

    // preset
    bResultTmp = TRUE;

    for (i = 0; i < (PNDEV_PCI_CONFIG_SPACE__SIZE / sizeof(UINT32)); i++)
        // complete ConfigSpace
    {
        // get value of ConfigSpace
        lResultTmp = adn_pci_read_config_uint32(pDevExtIn->uPciLoc.uNo.lBus,
                                                pDevExtIn->uPciLoc.uNo.lDevice,
                                                pDevExtIn->uPciLoc.uNo.lFunction,
                                                (i*sizeof(UINT32)),
                                                &lArrayConfigSpaceOut[i]);

        if  (lResultTmp != ADN_OK)
            // error
        {
            bResultTmp = FALSE;

            // set ErrorString
            fnBuildString(  _TEXT(__FUNCTION__),
                            _TEXT("(): Error at reading ConfigSpace!"),
                            NULL,
                            lDimErrorIn,
                            sErrorOut);

            // leave loop
            break;
        }
    }

    if  (bResultTmp)
        // success
    {
    uint16_t lCommand = 0;

        // get command of ConfigSpace
        lResultTmp = adn_pci_read_config_uint16(pDevExtIn->uPciLoc.uNo.lBus,
                                                pDevExtIn->uPciLoc.uNo.lDevice,
                                                pDevExtIn->uPciLoc.uNo.lFunction,
                                                ADN_PCI_CFG_COMMAND,
                                                &lCommand);

        if  (lResultTmp != ADN_OK)
            // error
        {
            // set ErrorString
            fnBuildString(  _TEXT(__FUNCTION__),
                            _TEXT("(): Error at reading command of ConfigSpace!"),
                            NULL,
                            lDimErrorIn,
                            sErrorOut);
        }
        else
        {
            // disable BusMaster
            lCommand &= PNDEV_UINT32_CLR_BIT_2;

            // update command at ConfigSpace
            lResultTmp = adn_pci_write_config_uint16(   pDevExtIn->uPciLoc.uNo.lBus,
                                                        pDevExtIn->uPciLoc.uNo.lDevice,
                                                        pDevExtIn->uPciLoc.uNo.lFunction,
                                                        ADN_PCI_CFG_COMMAND,
                                                        lCommand);

            if  (lResultTmp != ADN_OK)
                // error
            {
                // set ErrorString
                fnBuildString(  _TEXT(__FUNCTION__),
                                _TEXT("Error at writing Command at ConfigSpace!"),
                                NULL,
                                lDimErrorIn,
                                sErrorOut);
            }
            else
            {
                // success
                bResult = TRUE;
            }
        }
    }

    return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN fnPrepareConfigSpaceHwReset(DEVICE_EXTENSION*   pDevExtIn,
                                    const UINT32        lDimErrorIn,
                                    UINT32              lArrayConfigSpaceOut[],     // array=ptr
                                    _TCHAR*             sErrorOut)
{
BOOLEAN bResult     = FALSE;
BOOLEAN bResultTmp  = FALSE;
UINT32  lResultTmp  = LINUX_ERROR;
UINT32  i = 0;

// preset
    bResultTmp = TRUE;

    for (i = 0; i < (PNDEV_PCI_CONFIG_SPACE__SIZE / sizeof(UINT32)); i++)
        // complete ConfigSpace
    {
        // get value of ConfigSpace
        lResultTmp = pci_read_config_dword( pDevExtIn->pPciDev,
                                            (i*sizeof(UINT32)),
                                            &lArrayConfigSpaceOut[i]);

        if  (lResultTmp != LINUX_OK)
            // error
        {
            bResultTmp = FALSE;

            // set ErrorString
            fnBuildString(  _TEXT(__FUNCTION__),
                            _TEXT("(): Error at reading ConfigSpace!"),
                            NULL,
                            lDimErrorIn,
                            sErrorOut);

            // leave loop
            break;
        }
    }

    if  (bResultTmp)
        // success
    {
    uint16_t lCommand = 0;

        // get command of ConfigSpace
        lResultTmp = pci_read_config_word(  pDevExtIn->pPciDev,
                                            PCI_COMMAND,
                                            &lCommand);

        if  (lResultTmp != LINUX_OK)
            // error
        {
            // set ErrorString
            fnBuildString(  _TEXT(__FUNCTION__),
                            _TEXT("(): Error at reading command of ConfigSpace!"),
                            NULL,
                            lDimErrorIn,
                            sErrorOut);
        }
        else
        {
            // disable BusMaster
            lCommand &= PNDEV_UINT32_CLR_BIT_2;

            // update command at ConfigSpace
            lResultTmp = pci_write_config_word( pDevExtIn->pPciDev,
                                                PCI_COMMAND,
                                                lCommand);

            if  (lResultTmp != LINUX_OK)
                // error
            {
                // set ErrorString
                fnBuildString(  _TEXT(__FUNCTION__),
                                _TEXT("Error at writing Command at ConfigSpace!"),
                                NULL,
                                lDimErrorIn,
                                sErrorOut);
            }
            else
            {
                // success
                bResult = TRUE;
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
//  restore ConfigSpace after HwReset
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN fnRestoreConfigSpaceHwReset(DEVICE_EXTENSION*   pDevExtIn,
                                    UINT32              lArrayConfigSpaceIn[],      // array=ptr
                                    const UINT32        lDimErrorIn,
                                    _TCHAR*             sErrorOut)
{
BOOLEAN                 bResult     = FALSE;
NTSTATUS                lResultTmp1 = STATUS_UNSUCCESSFUL;
BUS_INTERFACE_STANDARD  uBusInterface;

    lResultTmp1 = WdfFdoQueryForInterface(  pDevExtIn->hDeviceOs,
                                            &GUID_BUS_INTERFACE_STANDARD,
                                            (PINTERFACE) &uBusInterface,
                                            sizeof(BUS_INTERFACE_STANDARD),
                                            1,                                      // Version
                                            NULL);                                  // InterfaceSpecificData

    if  (lResultTmp1 != STATUS_SUCCESS)
        // error
    {
        // set ErrorString
        fnBuildString(  _TEXT(__FUNCTION__),
                        _TEXT("(): Error at acquiring BusInterface!"),
                        NULL,
                        lDimErrorIn,
                        sErrorOut);
    }
    else
    {
    ULONG lResultTmp2 = 0;

        // write ConfigSpace
        lResultTmp2 = uBusInterface.SetBusData( uBusInterface.Context,
                                                PCI_WHICHSPACE_CONFIG,
                                                lArrayConfigSpaceIn,
                                                0,                              // offset into ConfigSpace
                                                PNDEV_PCI_CONFIG_SPACE__SIZE);

        if  (lResultTmp2 != PNDEV_PCI_CONFIG_SPACE__SIZE)
            // error
        {
            // set ErrorString
            fnBuildString(  _TEXT(__FUNCTION__),
                            _TEXT("(): Error at writing ConfigSpace!"),
                            NULL,
                            lDimErrorIn,
                            sErrorOut);
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
BOOLEAN fnRestoreConfigSpaceHwReset(DEVICE_EXTENSION*   pDevExtIn,
                                    UINT32              lArrayConfigSpaceIn[],      // array=ptr
                                    const UINT32        lDimErrorIn,
                                    _TCHAR*             sErrorOut)
{
BOOLEAN bResult     = FALSE;
int32_t lResultTmp  = ADN_ERROR;
UINT32  i = 0;

    // preset
    bResult = TRUE;

    for (i = 0; i < (PNDEV_PCI_CONFIG_SPACE__SIZE / sizeof(UINT32)); i++)
        // complete ConfigSpace
    {
        // update value at ConfigSpace
        lResultTmp = adn_pci_write_config_uint32(   pDevExtIn->uPciLoc.uNo.lBus,
                                                    pDevExtIn->uPciLoc.uNo.lDevice,
                                                    pDevExtIn->uPciLoc.uNo.lFunction,
                                                    (i*sizeof(UINT32)),
                                                    lArrayConfigSpaceIn[i]);

        if  (lResultTmp != ADN_OK)
            // error
        {
            bResult = FALSE;

            // set ErrorString
            fnBuildString(  _TEXT(__FUNCTION__),
                            _TEXT("(): Error at writing ConfigSpace!"),
                            NULL,
                            lDimErrorIn,
                            sErrorOut);

            // leave loop
            break;
        }
    }

    return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN fnRestoreConfigSpaceHwReset(DEVICE_EXTENSION*   pDevExtIn,
                                    UINT32              lArrayConfigSpaceIn[],      // array=ptr
                                    const UINT32        lDimErrorIn,
                                    _TCHAR*             sErrorOut)
{
BOOLEAN bResult     = FALSE;
int32_t lResultTmp  = LINUX_ERROR;
UINT32  i = 0;

    // preset
    bResult = TRUE;

    for (i = 0; i < (PNDEV_PCI_CONFIG_SPACE__SIZE / sizeof(UINT32)); i++)
        // complete ConfigSpace
    {
        // update value at ConfigSpace
        lResultTmp = pci_write_config_dword(pDevExtIn->pPciDev,
                                            (i*sizeof(UINT32)),
                                            lArrayConfigSpaceIn[i]);

        if  (lResultTmp != LINUX_OK)
            // error
        {
            bResult = FALSE;

            // set ErrorString
            fnBuildString(  _TEXT(__FUNCTION__),
                            _TEXT("(): Error at writing ConfigSpace!"),
                            NULL,
                            lDimErrorIn,
                            sErrorOut);

            // leave loop
            break;
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
//  Enable BusMuster for Springville and Hartwell ConfigSpace
//
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN fnEnableBusMasterConfigSpace(   DEVICE_EXTENSION*   pDevExtIn,
                                        const UINT32        lDimErrorIn,
                                        _TCHAR*             sErrorOut)
{
BOOLEAN   bResult     = FALSE;

    bResult = TRUE;

    return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
BOOLEAN fnEnableBusMasterConfigSpace(   DEVICE_EXTENSION*   pDevExtIn,
                                        const UINT32        lDimErrorIn,
                                        _TCHAR*             sErrorOut)
{
BOOLEAN bResult = FALSE;

    bResult = TRUE;

    return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN fnEnableBusMasterConfigSpace(   DEVICE_EXTENSION*   pDevExtIn,
                                        const UINT32        lDimErrorIn,
                                        _TCHAR*             sErrorOut)
{
BOOLEAN bResult     = FALSE;
UINT32  lResultTmp  = LINUX_ERROR;
UINT16  lCommand    = 0;

    if  (   (pDevExtIn->eBoard == ePNDEV_BOARD_I210)
        ||  (pDevExtIn->eBoard == ePNDEV_BOARD_I82574)
        ||  (pDevExtIn->eBoard == ePNDEV_BOARD_MICREL_KSZ8841)
        ||  (pDevExtIn->eBoard == ePNDEV_BOARD_MICREL_KSZ8842)
    	||  (pDevExtIn->eBoard == ePNDEV_BOARD_TI_OMAP))
        // standard card
    {
        // get command of ConfigSpace
        lResultTmp = pci_read_config_word(  pDevExtIn->pPciDev,
                                            PCI_COMMAND,
                                            &lCommand);

        if  (lResultTmp != LINUX_OK)
            // error
        {
            // set ErrorString
            fnBuildString(  _TEXT(__FUNCTION__),
                            _TEXT("(): Error at reading command of ConfigSpace!"),
                            NULL,
                            lDimErrorIn,
                            sErrorOut);
        }
        else
        {
            if  ((lCommand >> 2) & 1)
                // BusMaster already enabled
            {
                // nothing to do
                bResult = TRUE;
            }
            else
            {
                // enable BusMaster
                lCommand |= PNDEV_UINT32_SET_BIT_2;
    
                // update command at ConfigSpace
                lResultTmp = pci_write_config_word( pDevExtIn->pPciDev,
                                                    PCI_COMMAND,
                                                    lCommand);
    
                if  (lResultTmp != LINUX_OK)
                    // error
                {
                    // set ErrorString
                    fnBuildString(  _TEXT(__FUNCTION__),
                                    _TEXT("Error at writing Command at ConfigSpace!"),
                                    NULL,
                                    lDimErrorIn,
                                    sErrorOut);
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

    return(bResult);
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  Disable BusMuster for Springville and Hartwell ConfigSpace
//
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN fnDisableBusMasterConfigSpace(  DEVICE_EXTENSION*   pDevExtIn,
                                        const UINT32        lDimErrorIn,
                                        _TCHAR*             sErrorOut)
{
BOOLEAN   bResult     = FALSE;

    bResult = TRUE;

    return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
BOOLEAN fnDisableBusMasterConfigSpace(  DEVICE_EXTENSION*   pDevExtIn,
                                        const UINT32        lDimErrorIn,
                                        _TCHAR*             sErrorOut)
{
BOOLEAN bResult = FALSE;

    bResult = TRUE;

    return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN fnDisableBusMasterConfigSpace(  DEVICE_EXTENSION*   pDevExtIn,
                                        const UINT32        lDimErrorIn,
                                        _TCHAR*             sErrorOut)
{
BOOLEAN bResult     = FALSE;
UINT32  lResultTmp  = LINUX_ERROR;
UINT16  lCommand    = 0;

    if  (   (pDevExtIn->eBoard == ePNDEV_BOARD_I210)
        ||  (pDevExtIn->eBoard == ePNDEV_BOARD_I82574)
        ||  (pDevExtIn->eBoard == ePNDEV_BOARD_MICREL_KSZ8841)
        ||  (pDevExtIn->eBoard == ePNDEV_BOARD_MICREL_KSZ8842)
        ||  (pDevExtIn->eBoard == ePNDEV_BOARD_TI_OMAP))
        // standard card
    {
        // get command of ConfigSpace
        lResultTmp = pci_read_config_word(  pDevExtIn->pPciDev,
                                            PCI_COMMAND,
                                            &lCommand);

        if  (lResultTmp != LINUX_OK)
            // error
        {
            // set ErrorString
            fnBuildString(  _TEXT(__FUNCTION__),
                            _TEXT("(): Error at reading command of ConfigSpace!"),
                            NULL,
                            lDimErrorIn,
                            sErrorOut);
        }
        else
        {
            if  (!((lCommand >> 2) & 1))
                // BusMaster already disabled
            {
                // nothing to do
                bResult = TRUE;
            }
            else
            {
                // disable BusMaster
                lCommand &= PNDEV_UINT32_CLR_BIT_2;
    
                // update command at ConfigSpace
                lResultTmp = pci_write_config_word( pDevExtIn->pPciDev,
                                                    PCI_COMMAND,
                                                    lCommand);
    
                if  (lResultTmp != LINUX_OK)
                    // error
                {
                    // set ErrorString
                    fnBuildString(  _TEXT(__FUNCTION__),
                                    _TEXT("Error at writing Command at ConfigSpace!"),
                                    NULL,
                                    lDimErrorIn,
                                    sErrorOut);
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

    return(bResult);
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  read BitStreamVersion from ConfigSpace
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
UINT32 fnReadConfigSpaceBitStreamVer(	DEVICE_EXTENSION*   pDevExtIn,
    									UINT32				lOffsetIn,
    									const UINT32        lDimErrorIn,
    									_TCHAR*             sErrorOut)
{
UINT32 					lBitStreamVersion 	= 0;
NTSTATUS                lResultTmp  = STATUS_UNSUCCESSFUL;
BUS_INTERFACE_STANDARD  uBusInterface;

    lResultTmp = WdfFdoQueryForInterface(   pDevExtIn->hDeviceOs,
                                            &GUID_BUS_INTERFACE_STANDARD,
                                            (PINTERFACE) &uBusInterface,
                                            sizeof(BUS_INTERFACE_STANDARD),
                                            1,                                      // Version
                                            NULL);                                  // InterfaceSpecificData

    if  (lResultTmp != STATUS_SUCCESS)
        // error
    {
        // set ErrorString
        fnBuildString(  _TEXT(__FUNCTION__),
                        _TEXT("(): Error at acquiring BusInterface!"),
                        NULL,
                        lDimErrorIn,
                        sErrorOut);
    }
    else
    {
    PCI_COMMON_HEADER   uHeader;
    PCI_COMMON_CONFIG*  pPciConfig  = (PCI_COMMON_CONFIG*) &uHeader;

        // PCI_COMMON_HEADER structure: used to retrieve the 64 bytes of data that precedes the device-specific data
        // PCI_COMMON_CONFIG structure: includes device specific data

        // read first part of the header to get the StatusRegister and the CapabilitiesPtr
        //  - CapabilitiesPtr is an offset from the beginning of the header to a linked list of capabilities
        uBusInterface.GetBusData(   uBusInterface.Context,
                                    PCI_WHICHSPACE_CONFIG,
                                    pPciConfig,
                                    0,
                                    sizeof(PCI_COMMON_HEADER));

        if  (pPciConfig->Status & PCI_STATUS_CAPABILITIES_LIST)
            // CapabilityList present
        {
			// read BitStreamVersion from Configspace
			uBusInterface.GetBusData(   uBusInterface.Context,
										PCI_WHICHSPACE_CONFIG,
										&lBitStreamVersion,
										lOffsetIn,
										sizeof(UINT32));
        }
    }

    return(lBitStreamVersion);
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
UINT32 fnReadConfigSpaceBitStreamVer(	DEVICE_EXTENSION*   pDevExtIn,
    									UINT32				lOffsetIn,
    									const UINT32        lDimErrorIn,
    									_TCHAR*             sErrorOut)
{
UINT32 lBitStreamVersion = 0;

	return lBitStreamVersion;
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
UINT32 fnReadConfigSpaceBitStreamVer(	DEVICE_EXTENSION*   pDevExtIn,
    									UINT32				lOffsetIn,
    									const UINT32        lDimErrorIn,
    									_TCHAR*             sErrorOut)
{
UINT32 lBitStreamVersion = 0;

	return lBitStreamVersion;
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  check pending Transaction of ExtendedConfigSpace from PCIe
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN fnCheckTransactionPending(	DEVICE_EXTENSION*	pDevExtIn,
                                	const UINT32        lDimErrorIn,
                                	_TCHAR*             sErrorOut)
{
BOOLEAN                 bResult     = FALSE;
NTSTATUS                lResultTmp  = STATUS_UNSUCCESSFUL;
BUS_INTERFACE_STANDARD  uBusInterface;

    lResultTmp = WdfFdoQueryForInterface(   pDevExtIn->hDeviceOs,
                                            &GUID_BUS_INTERFACE_STANDARD,
                                            (PINTERFACE) &uBusInterface,
                                            sizeof(BUS_INTERFACE_STANDARD),
                                            1,                                      // Version
                                            NULL);                                  // InterfaceSpecificData

    if  (lResultTmp != STATUS_SUCCESS)
        // error
    {
        // set ErrorString
        fnBuildString(  _TEXT(__FUNCTION__),
                        _TEXT("(): Error at acquiring BusInterface!"),
                        NULL,
                        lDimErrorIn,
                        sErrorOut);
    }
    else
    {
    PCI_COMMON_HEADER   uHeader;
    PCI_COMMON_CONFIG*  pPciConfig  = (PCI_COMMON_CONFIG*) &uHeader;

        // success
        bResult = TRUE;

        // PCI_COMMON_HEADER structure: used to retrieve the 64 bytes of data that precedes the device-specific data
        // PCI_COMMON_CONFIG structure: includes device specific data

        // read first part of the header to get the StatusRegister and the CapabilitiesPtr
        //  - CapabilitiesPtr is an offset from the beginning of the header to a linked list of capabilities
        uBusInterface.GetBusData(   uBusInterface.Context,
                                    PCI_WHICHSPACE_CONFIG,
                                    pPciConfig,
                                    0,
                                    sizeof(PCI_COMMON_HEADER));

        if  (pPciConfig->Status & PCI_STATUS_CAPABILITIES_LIST)
            // CapabilityList present
        {
        PCI_EXPRESS_CAPABILITY  uPciExpressCapability;
        UINT32                  lOffsetCapability   = 0;
        UINT32                  lIntLine            = 0;
        UINT32                  lIntPin             = 0;
        BOOLEAN                 bDebug              = FALSE;

            // device supports capability lists -> find the capabilities

            // get offset to CapabilitiesPtr
            //  - position of the CapabilitiesPtr in the header depends on whether this is a BridgeTypeDevice
            {
                if  ((pPciConfig->HeaderType & (~PCI_MULTIFUNCTION)) == PCI_BRIDGE_TYPE)
                    // PciConfigType = PCI to PCI Bridge
                {
                    lOffsetCapability = pPciConfig->u.type1.CapabilitiesPtr;
                }
                else if ((pPciConfig->HeaderType & (~PCI_MULTIFUNCTION)) == PCI_CARDBUS_BRIDGE_TYPE)
                        // PciConfigType = PCI to CARDBUS Bridge
                {
                    lOffsetCapability = pPciConfig->u.type2.CapabilitiesPtr;
                }
                else
                {
                    lOffsetCapability = pPciConfig->u.type0.CapabilitiesPtr;
                }
            }

            // loop through the capabilities in search of the desired capability
            //  - the list is NULL-terminated, so the last offset will always be zero

            while   (lOffsetCapability)
                    // not end of list
            {
                // read the header of the capability at this offset
                uBusInterface.GetBusData(   uBusInterface.Context,
                                            PCI_WHICHSPACE_CONFIG,
                                            &uPciExpressCapability,
                                            lOffsetCapability,
                                            sizeof(PCI_CAPABILITIES_HEADER));

                if  (uPciExpressCapability.Header.CapabilityID == PCI_CAPABILITY_ID_PCI_EXPRESS)
                    // PCIe capability found
                {
                    // leave loop
                    break;
                }
                else
                {
                    // look to next capability
                    lOffsetCapability = uPciExpressCapability.Header.Next;
                }
            }

            if  (lOffsetCapability != 0)
                // PCIe CapabilityList found
            {
            UINT32 lWaitTmp = 0;
            
                // read PCIe CapabilityList
                uBusInterface.GetBusData(   uBusInterface.Context,
                                            PCI_WHICHSPACE_CONFIG,
                                            &uPciExpressCapability.ExpressCapabilities,
                                            lOffsetCapability + sizeof(PCI_CAPABILITIES_HEADER),
                                            sizeof(PCI_EXPRESS_CAPABILITY) - sizeof(PCI_CAPABILITIES_HEADER));
                
				while	(uPciExpressCapability.DeviceStatus.TransactionsPending)
						// TransactionPending is true
				{
					// wait 100ms
					if	(fnDelayThread(	pDevExtIn,
										100,
										lDimErrorIn,
										sErrorOut))
						// starting WaitTime of current thread ok
					{
						lWaitTmp++;
						
						if (lWaitTmp == 5)
							// don't wait
						{
							// set ErrorString
							fnBuildString(  _TEXT(__FUNCTION__),
							                _TEXT("(): Error at Waiting for TransactionPending!"),
							                NULL,
							                lDimErrorIn,
							                sErrorOut);
							
							// stop waiting
							break;							
						}	
					}
				}
				
	            if	(!uPciExpressCapability.DeviceStatus.TransactionsPending)
	            	// no pending transaction exists
	            {
	            	// success
	            	bResult = TRUE;
	            }
            }
            else
            {
            	// set ErrorString
            	fnBuildString(  _TEXT(__FUNCTION__),
            	                _TEXT("(): No PciExpressCapability was found in ConfigSpace!"),
            	                NULL,
            	                lDimErrorIn,
            	                sErrorOut);
            }
        }
    }

    return(bResult);
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
BOOLEAN fnCheckTransactionPending(	DEVICE_EXTENSION*	pDevExtIn,
                                	const UINT32        lDimErrorIn,
                                	_TCHAR*             sErrorOut)
{
BOOLEAN bResult = FALSE;

	bResult = TRUE;

    return(bResult);
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN fnCheckTransactionPending(	DEVICE_EXTENSION*	pDevExtIn,
                                	const UINT32        lDimErrorIn,
                                	_TCHAR*             sErrorOut)
{
BOOLEAN bResult = FALSE;

	bResult = TRUE;

    return(bResult);
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  check pending Transaction of ExtendedConfigSpace from PCIe
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN fnSetFlrAtConfigSpace(	DEVICE_EXTENSION*	pDevExtIn,
                               	const UINT32        lDimErrorIn,
                               	_TCHAR*             sErrorOut)
{
BOOLEAN                 bResult     = FALSE;
NTSTATUS                lResultTmp  = STATUS_UNSUCCESSFUL;
BUS_INTERFACE_STANDARD  uBusInterface;

    lResultTmp = WdfFdoQueryForInterface(   pDevExtIn->hDeviceOs,
                                            &GUID_BUS_INTERFACE_STANDARD,
                                            (PINTERFACE) &uBusInterface,
                                            sizeof(BUS_INTERFACE_STANDARD),
                                            1,                                      // Version
                                            NULL);                                  // InterfaceSpecificData

    if  (lResultTmp != STATUS_SUCCESS)
        // error
    {
        // set ErrorString
        fnBuildString(  _TEXT(__FUNCTION__),
                        _TEXT("(): Error at acquiring BusInterface!"),
                        NULL,
                        lDimErrorIn,
                        sErrorOut);
    }
    else
    {
    PCI_COMMON_HEADER   uHeader;
    PCI_COMMON_CONFIG*  pPciConfig  = (PCI_COMMON_CONFIG*) &uHeader;

        // success
        bResult = TRUE;

        // PCI_COMMON_HEADER structure: used to retrieve the 64 bytes of data that precedes the device-specific data
        // PCI_COMMON_CONFIG structure: includes device specific data

        // read first part of the header to get the StatusRegister and the CapabilitiesPtr
        //  - CapabilitiesPtr is an offset from the beginning of the header to a linked list of capabilities
        uBusInterface.GetBusData(   uBusInterface.Context,
                                    PCI_WHICHSPACE_CONFIG,
                                    pPciConfig,
                                    0,
                                    sizeof(PCI_COMMON_HEADER));

        if  (pPciConfig->Status & PCI_STATUS_CAPABILITIES_LIST)
            // CapabilityList present
        {
        PCI_EXPRESS_CAPABILITY  uPciExpressCapability;
        UINT32                  lOffsetCapability   = 0;
        UINT32                  lIntLine            = 0;
        UINT32                  lIntPin             = 0;
        BOOLEAN                 bDebug              = FALSE;

            // device supports capability lists -> find the capabilities

            // get offset to CapabilitiesPtr
            //  - position of the CapabilitiesPtr in the header depends on whether this is a BridgeTypeDevice
            {
                if  ((pPciConfig->HeaderType & (~PCI_MULTIFUNCTION)) == PCI_BRIDGE_TYPE)
                    // PciConfigType = PCI to PCI Bridge
                {
                    lOffsetCapability = pPciConfig->u.type1.CapabilitiesPtr;
                }
                else if ((pPciConfig->HeaderType & (~PCI_MULTIFUNCTION)) == PCI_CARDBUS_BRIDGE_TYPE)
                        // PciConfigType = PCI to CARDBUS Bridge
                {
                    lOffsetCapability = pPciConfig->u.type2.CapabilitiesPtr;
                }
                else
                {
                    lOffsetCapability = pPciConfig->u.type0.CapabilitiesPtr;
                }
            }

            // loop through the capabilities in search of the desired capability
            //  - the list is NULL-terminated, so the last offset will always be zero

            while   (lOffsetCapability)
                    // not end of list
            {
                // read the header of the capability at this offset
                uBusInterface.GetBusData(   uBusInterface.Context,
                                            PCI_WHICHSPACE_CONFIG,
                                            &uPciExpressCapability,
                                            lOffsetCapability,
                                            sizeof(PCI_CAPABILITIES_HEADER));

                if  (uPciExpressCapability.Header.CapabilityID == PCI_CAPABILITY_ID_PCI_EXPRESS)
                    // PCIe capability found
                {
                    // leave loop
                    break;
                }
                else
                {
                    // look to next capability
                    lOffsetCapability = uPciExpressCapability.Header.Next;
                }
            }

            if  (lOffsetCapability != 0)
                // PCIe CapabilityList found
            {
            UINT32 lResultTmp = 0;
            
                // read PCIe CapabilityList
                uBusInterface.GetBusData(   uBusInterface.Context,
                                            PCI_WHICHSPACE_CONFIG,
                                            &uPciExpressCapability.ExpressCapabilities,
                                            lOffsetCapability + sizeof(PCI_CAPABILITIES_HEADER),
                                            sizeof(PCI_EXPRESS_CAPABILITY) - sizeof(PCI_CAPABILITIES_HEADER));
                
                // set FLR
                uPciExpressCapability.DeviceControl.BridgeConfigRetryEnable = 1;
                
                //  - write DeviceStatus back to PCIe CapabilityList
                lResultTmp = uBusInterface.SetBusData(	uBusInterface.Context,
                										PCI_WHICHSPACE_CONFIG,
                										&uPciExpressCapability.DeviceControl.AsUSHORT,
                										lOffsetCapability + FIELD_OFFSET(PCI_EXPRESS_CAPABILITY, DeviceControl),
                										sizeof(USHORT));
                
                if  (lResultTmp !=  sizeof(USHORT))
                    // error
                {
                    // set ErrorString
                    fnBuildString(  _TEXT(__FUNCTION__),
                                    _TEXT("(): Error at writing FLR at ExtConfigSpace DeviceControl!"),
                                    NULL,
                                    lDimErrorIn,
                                    sErrorOut);
                }
                else
                {
                    // success
                    bResult = TRUE;
                }
            }
            else
            {
            	// set ErrorString
            	fnBuildString(  _TEXT(__FUNCTION__),
            	                _TEXT("(): No PciExpressCapability was found in ConfigSpace!"),
            	                NULL,
            	                lDimErrorIn,
            	                sErrorOut);
            }
        }
    }

    return(bResult);
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
BOOLEAN fnSetFlrAtConfigSpace(	DEVICE_EXTENSION*	pDevExtIn,
                                const UINT32        lDimErrorIn,
                                _TCHAR*             sErrorOut)
{
BOOLEAN bResult = FALSE;

	bResult = TRUE;

    return(bResult);
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN fnSetFlrAtConfigSpace(	DEVICE_EXTENSION*	pDevExtIn,
                               	const UINT32        lDimErrorIn,
                               	_TCHAR*             sErrorOut)
{
BOOLEAN bResult = FALSE;

	bResult = TRUE;

    return(bResult);
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  check Vendor-/DeviceId
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN fnCheckVendorDeviceIdAtConfigSpace(	DEVICE_EXTENSION*	pDevExtIn,
											const UINT16		lVendorIdIn,
											const UINT16		lDeviceIdIn,
											const UINT32        lDimErrorIn,
											_TCHAR*             sErrorOut)
{
BOOLEAN                 bResult     = FALSE;
NTSTATUS                lResultTmp  = STATUS_UNSUCCESSFUL;
BUS_INTERFACE_STANDARD  uBusInterface;

	lResultTmp = WdfFdoQueryForInterface(   pDevExtIn->hDeviceOs,
											&GUID_BUS_INTERFACE_STANDARD,
											(PINTERFACE) &uBusInterface,
											sizeof(BUS_INTERFACE_STANDARD),
											1,                                      // Version
											NULL);                                  // InterfaceSpecificData
	
	if  (lResultTmp != STATUS_SUCCESS)
		// error
	{
		// set ErrorString
		fnBuildString(  _TEXT(__FUNCTION__),
						_TEXT("(): Error at acquiring BusInterface!"),
						NULL,
						lDimErrorIn,
						sErrorOut);
	}
	else
	{
	PCI_COMMON_HEADER   uHeader;
	PCI_COMMON_CONFIG*  pPciConfig  = (PCI_COMMON_CONFIG*) &uHeader;
	
		// PCI_COMMON_HEADER structure: used to retrieve the 64 bytes of data that precedes the device-specific data
		// PCI_COMMON_CONFIG structure: includes device specific data
	
		// read first part of the header to get the StatusRegister and the CapabilitiesPtr
		//  - CapabilitiesPtr is an offset from the beginning of the header to a linked list of capabilities
		uBusInterface.GetBusData(   uBusInterface.Context,
									PCI_WHICHSPACE_CONFIG,
									pPciConfig,
									0,
									sizeof(PCI_COMMON_HEADER));
	
		if	(	(pPciConfig->VendorID == lVendorIdIn)
			&&	(pPciConfig->DeviceID == lDeviceIdIn))
			// Vendor and DeviceId correct
		{
			// success
			bResult = TRUE;
		}
		else
		{
			// set ErrorString
			fnBuildString(  _TEXT(__FUNCTION__),
							_TEXT("(): Error at Checking VendorId and DeviceId after FLR!"),
							NULL,
							lDimErrorIn,
							sErrorOut);
		}
	}

    return(bResult);
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
BOOLEAN fnCheckVendorDeviceIdAtConfigSpace(	DEVICE_EXTENSION*	pDevExtIn,
											const UINT16		lVendorIdIn,
											const UINT16		lDeviceIdIn,
											const UINT32        lDimErrorIn,
											_TCHAR*             sErrorOut)
{
BOOLEAN bResult = FALSE;

	bResult = TRUE;

    return(bResult);
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN fnCheckVendorDeviceIdAtConfigSpace(	DEVICE_EXTENSION*	pDevExtIn,
											const UINT16		lVendorIdIn,
											const UINT16		lDeviceIdIn,
											const UINT32        lDimErrorIn,
											_TCHAR*             sErrorOut)
{
BOOLEAN bResult = FALSE;

	bResult = TRUE;

    return(bResult);
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  scan for desired device
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
BOOLEAN fnScanDevice(   const UINT32        lVendorIdIn,
                        const UINT32        lDeviceIdIn,
                        const UINT32        lSubsystemIdIn,
                        const ePNDEV_BOARD  eBoardIn)
{
BOOLEAN     bResult     = FALSE;
uint16_t    lIndex      = 0;
uint8_t     lNoBus      = 0;
uint16_t    lNoDevice   = 0;
uint16_t    lNoFct      = 0;
char        sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

    // preset
    bResult = TRUE;

    while   (adn_pci_find_device(   (uint16_t) lVendorIdIn,
                                    (uint16_t) lDeviceIdIn,
                                    lIndex,
                                    &lNoBus,
                                    &lNoDevice,
                                    &lNoFct) == ADN_OK)
            // new PCI device found
    {
    BOOLEAN bResultSubsystem = TRUE;

        if  (lSubsystemIdIn != 0)
            // SubsystemId should be checked
        {
        int32_t     lResultTmp      = ADN_ERROR;
        uint16_t    lSubsystemId    = 0;

            // get SubsystemId
            lResultTmp = adn_pci_read_config_uint16(lNoBus,
                                                    lNoDevice,
                                                    lNoFct,
                                                    ADN_PCI_CFG_SUB_SYSTEM_ID,
                                                    &lSubsystemId);

            if  (lResultTmp != ADN_OK)
                // error
            {
                bResult = FALSE;

                // set TraceDetail
                RtlStringCchPrintfA(sTraceDetail,
                                    _countof(sTraceDetail),
                                    "PnDev[x,x,x]: ########## Error at getting SubsystemId!");

                // set trace
                fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                            ePNDEV_TRACE_CONTEXT_BASIC,
                            TRUE,                       // logging
                            sTraceDetail);

                // leave loop
                break;
            }
            else
            {
                if  (lSubsystemId != lSubsystemIdIn)
                    // not board with desired SubsystemId
                {
                    bResultSubsystem = FALSE;
                }
            }
        }

        if  (bResultSubsystem)
            // success
        {
        DEVICE_EXTENSION* pDevExt = NULL;

            // allocate DeviceExtension
            pDevExt = fnAllocMemNonPaged(   sizeof(DEVICE_EXTENSION),
                                            TAG('I','n','i','t'));

            if  (pDevExt == NULL)
                // error
            {
                bResult = FALSE;

                // set TraceDetail
                RtlStringCchPrintfA(sTraceDetail,
                                    _countof(sTraceDetail),
                                    "PnDev[x,x,x]: ########## Insufficient memory for DeviceExtension!");

                // set trace
                fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                            ePNDEV_TRACE_CONTEXT_BASIC,
                            TRUE,                       // logging
                            sTraceDetail);

                // leave loop
                break;
            }
            else
            {
                // preset DeviceExtension
                RtlZeroMemory(  (void*) pDevExt,
                                sizeof(DEVICE_EXTENSION));

                if  (!fnEvtDeviceAdd(   pDevExt,
                                        eBoardIn,
                                        lNoBus,
                                        lNoDevice,
                                        lNoFct))
                    // error at CBF for AddDevice
                {
                    bResult = FALSE;

                    // leave loop
                    break;
                }
                else
                {
                    if  (!fnEvtDevicePrepareHw(pDevExt))
                        // error at CBF for preparing hardware
                    {
                        bResult = FALSE;

                        // leave loop
                        break;
                    }
                    else
                    {
                        // put DeviceExtension to DeviceList
                        fnPutBlkToList( &g_uVarSys.uListDevice,
                                        ((uLIST_HEADER*) pDevExt));

                        g_uVarSys.lCtrDevice++;
                    }
                }
            }
        }

        // next device
        lIndex++;
    }

    return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN fnScanDevice(   const UINT32        lVendorIdIn,
                        const UINT32        lDeviceIdIn,
                        const UINT32        lSubsystemIdIn,
                        const ePNDEV_BOARD  eBoardIn)
{
BOOLEAN         bResult     = FALSE;
struct pci_dev* pPciDev     = NULL;
struct pci_dev* pPciDevPrev = NULL;
char    sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

    // preset
    bResult = TRUE;

    do
    {
    BOOLEAN bResultSubsystem = TRUE;

        // get PCI device
        pPciDev = pci_get_device(   lVendorIdIn,
                                    lDeviceIdIn,
                                    pPciDevPrev);   // ptr to previous found device

        if  (pPciDev == NULL)
            // error
        {
            // ignore, because such device is not plugged
            // bResult = FALSE;

            // set TraceDetail
            RtlStringCchPrintfA(sTraceDetail,
                                _countof(sTraceDetail),
                                "PnDev[x,x,x]: ########## No Device found at pci_get_device()!");

            // set trace
            fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                        ePNDEV_TRACE_CONTEXT_BASIC,
                        TRUE,                       // logging
                        sTraceDetail);

            // leave loop
            break;
        }
        else
        {
        UINT32  lResultTmp = LINUX_ERROR;

			if	(!fnCheckPciLocIn(	PciLoc,
									PNDEV_ARRAY_DIM,
									pPciDev->bus->number,		// Bus
									PCI_SLOT(pPciDev->devfn),	// Device
									PCI_FUNC(pPciDev->devfn)))	// Func
				// device was not in PciLoc
			{
				// skip this Device
				bResult = TRUE;

				// set TraceDetail
				RtlStringCchPrintfA(sTraceDetail,
									_countof(sTraceDetail),
									"PnDev[x,x,x]: ########## Error at fnCheckPciLocIn()!");

				// set trace
				fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
							ePNDEV_TRACE_CONTEXT_BASIC,
							TRUE,						// logging
							sTraceDetail);

			}
			else
			{
				lResultTmp = pci_enable_device(pPciDev);

				if  (lResultTmp != LINUX_OK)
					// error
				{

					bResult = FALSE;

					// set TraceDetail
					RtlStringCchPrintfA(sTraceDetail,
										_countof(sTraceDetail),
										"PnDev[x,x,x]: ########## Error at pci_enable_device()!");

					// set trace
					fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
								ePNDEV_TRACE_CONTEXT_BASIC,
								TRUE,                       // logging
								sTraceDetail);

					// leave loop
					break;
				}
				else
				{
					if  (lSubsystemIdIn != 0)
						// SubsystemId should be checked
					{

					UINT16  lSubsystemId    = 0;

						// get SubsystemId
						lResultTmp = pci_read_config_word(  pPciDev,
															PCI_SUBSYSTEM_ID,
															&lSubsystemId);

						if  (lResultTmp != LINUX_OK)
							// error
						{
							bResult = FALSE;

							// set TraceDetail
							RtlStringCchPrintfA(sTraceDetail,
												_countof(sTraceDetail),
												"PnDev[x,x,x]: ########## Error at getting SubsystemId!");

							// set trace
							fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
										ePNDEV_TRACE_CONTEXT_BASIC,
										TRUE,                       // logging
										sTraceDetail);

							// leave loop
							break;
						}
						else
						{
							if  (lSubsystemId != lSubsystemIdIn)
								// not board with desired SubsystemId
							{
								bResultSubsystem = FALSE;
							}
						}
					}

					if  (bResultSubsystem)
						// success
					{
					DEVICE_EXTENSION* pDevExt = NULL;

						// allocate DeviceExtension
						pDevExt = fnAllocMemNonPaged(   sizeof(DEVICE_EXTENSION),
														TAG('I','n','i','t'));

						if  (pDevExt == NULL)
							// error
						{
							bResult = FALSE;

							// set TraceDetail
							RtlStringCchPrintfA(sTraceDetail,
												_countof(sTraceDetail),
												"PnDev[x,x,x]: ########## Insufficient memory for DeviceExtension!");

							// set trace
							fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
										ePNDEV_TRACE_CONTEXT_BASIC,
										TRUE,                       // logging
										sTraceDetail);

							// leave loop
							break;
						}
						else
						{
							// preset DeviceExtension
							RtlZeroMemory(  (void*) pDevExt,
											sizeof(DEVICE_EXTENSION));

							// save PciDev at DeviceExtention
							pDevExt->pPciDev = pPciDev;

							if  (!fnEvtDeviceAdd(   pDevExt,
													eBoardIn,
													pPciDev->bus->number,       // Bus
													PCI_SLOT(pPciDev->devfn),   // Device
													PCI_FUNC(pPciDev->devfn)))  // Fucntion
								// error at CBF for AddDevice
							{
								bResult = FALSE;

								// leave loop
								break;
							}
							else
							{
								if  (!fnEvtDevicePrepareHw(pDevExt))
									// error at CBF for preparing hardware
								{
									bResult = FALSE;

									// leave loop
									break;
								}
								else
								{
									// put DeviceExtension to DeviceList
									fnPutBlkToList( &g_uVarSys.uListDevice,
													((uLIST_HEADER*) pDevExt));

									g_uVarSys.lCtrDevice++;
								}
							}
						}
					}
				}
			}
        }

        // next device
        pPciDevPrev = pPciDev;
    }
    while   (pPciDev);
            // PciDev was found

    return(bResult);
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  get BoardType
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
ePNDEV_BOARD fnGetBoardType(const _TCHAR* sHardwareIdIn)
{
ePNDEV_BOARD    eResult = ePNDEV_BOARD_INVALID;
UNICODE_STRING  sUcHardwareId;
UNICODE_STRING  sUcHardwareId_Cp1616;
UNICODE_STRING  sUcHardwareId_DBEB400Pcie;
UNICODE_STRING  sUcHardwareId_Eb200;
UNICODE_STRING  sUcHardwareId_DbSoc1Pci;
UNICODE_STRING  sUcHardwareId_DbSoc1Pcie;
UNICODE_STRING  sUcHardwareId_Fpga1_Ertec400;
UNICODE_STRING  sUcHardwareId_Fpga1_Ertec200;
UNICODE_STRING  sUcHardwareId_Fpga1_Soc1;
UNICODE_STRING  sUcHardwareId_Fpga1_Ertec200P;
UNICODE_STRING  sUcHardwareId_Eb200P;
UNICODE_STRING  sUcHardwareId_I210;
UNICODE_STRING	sUcHardwareId_I82574;
UNICODE_STRING	sUcHardwareId_Fpga1_Hera;
UNICODE_STRING	sUcHardwareId_Micrel_KSZ8841;
UNICODE_STRING	sUcHardwareId_Micrel_KSZ8842;
UNICODE_STRING	sUcHardwareId_Ti_Omap;
char            sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

    // initialize Unicode strings
    {
        RtlInitUnicodeString(   &sUcHardwareId,
                                sHardwareIdIn);
        RtlInitUnicodeString(   &sUcHardwareId_Cp1616,
                                PNDEV_HW_ID_CP1616);
        RtlInitUnicodeString(   &sUcHardwareId_DBEB400Pcie,
                                PNDEV_HW_ID_DB_EB400_PCIE);
        RtlInitUnicodeString(   &sUcHardwareId_Eb200,
                                PNDEV_HW_ID_EB200);
        RtlInitUnicodeString(   &sUcHardwareId_DbSoc1Pci,
                                PNDEV_HW_ID_DB_SOC1_PCI);
        RtlInitUnicodeString(   &sUcHardwareId_DbSoc1Pcie,
                                PNDEV_HW_ID_DB_SOC1_PCIE);
        RtlInitUnicodeString(   &sUcHardwareId_Fpga1_Ertec200P,
                                PNDEV_HW_ID_FPGA1_ERTEC200P);
        RtlInitUnicodeString(   &sUcHardwareId_Eb200P,
                                PNDEV_HW_ID_EB200P);
        RtlInitUnicodeString(   &sUcHardwareId_I210,
                                PNDEV_HW_ID_I210);
		RtlInitUnicodeString(	&sUcHardwareId_I82574,
								PNDEV_HW_ID_I82574);
		RtlInitUnicodeString(	&sUcHardwareId_Fpga1_Hera,
								PNDEV_HW_ID_FPGA1_HERA);
		RtlInitUnicodeString(	&sUcHardwareId_Micrel_KSZ8841,
								PNDEV_HW_ID_MICREL_KSZ8841);
		RtlInitUnicodeString(	&sUcHardwareId_Micrel_KSZ8842,
								PNDEV_HW_ID_MICREL_KSZ8842);
		RtlInitUnicodeString(	&sUcHardwareId_Ti_Omap,
								PNDEV_HW_ID_TI_OMAP);
    }

    if  (RtlPrefixUnicodeString(    &sUcHardwareId_Cp1616,
                                    &sUcHardwareId,
                                    TRUE))          // TRUE: ignore case
        // CP1616 board
    {
        eResult = ePNDEV_BOARD_CP1616;

        // set TraceDetail
        RtlStringCchPrintfA(sTraceDetail,
                            _countof(sTraceDetail),
                            "PnDev[x,x,x]: ++++ CP1616 board");

        // set trace
        fnSetTrace( ePNDEV_TRACE_LEVEL_INFO,
                    ePNDEV_TRACE_CONTEXT_BASIC,
                    TRUE,                       // logging
                    sTraceDetail);
    }
    else if (RtlPrefixUnicodeString(&sUcHardwareId_DBEB400Pcie,
                                    &sUcHardwareId,
                                    TRUE))          // TRUE: ignore case
        // DB-EB400-PCIe board
    {
        eResult = ePNDEV_BOARD_DB_EB400_PCIE;

        // set TraceDetail
        RtlStringCchPrintfA(sTraceDetail,
                            _countof(sTraceDetail),
                            "PnDev[x,x,x]: ++++ DB-EB400-PCIe board");

        // set trace
        fnSetTrace( ePNDEV_TRACE_LEVEL_INFO,
                    ePNDEV_TRACE_CONTEXT_BASIC,
                    TRUE,                       // logging
                    sTraceDetail);
    }
    else if (RtlPrefixUnicodeString(&sUcHardwareId_Eb200,
                                    &sUcHardwareId,
                                    TRUE))          // TRUE: ignore case
        // EB200 board
    {
        eResult = ePNDEV_BOARD_EB200;

        // set TraceDetail
        RtlStringCchPrintfA(sTraceDetail,
                            _countof(sTraceDetail),
                            "PnDev[x,x,x]: ++++ EB200 board");

        // set trace
        fnSetTrace( ePNDEV_TRACE_LEVEL_INFO,
                    ePNDEV_TRACE_CONTEXT_BASIC,
                    TRUE,                       // logging
                    sTraceDetail);
    }
    else if (RtlPrefixUnicodeString(&sUcHardwareId_DbSoc1Pci,
                                    &sUcHardwareId,
                                    TRUE))          // TRUE: ignore case
        // DB-Soc1-PCI board
    {
        eResult = ePNDEV_BOARD_DB_SOC1_PCI;

        // set TraceDetail
        RtlStringCchPrintfA(sTraceDetail,
                            _countof(sTraceDetail),
                            "PnDev[x,x,x]: ++++ DB-Soc1-PCI board");

        // set trace
        fnSetTrace( ePNDEV_TRACE_LEVEL_INFO,
                    ePNDEV_TRACE_CONTEXT_BASIC,
                    TRUE,                       // logging
                    sTraceDetail);
    }
    else if (RtlPrefixUnicodeString(&sUcHardwareId_DbSoc1Pcie,
                                    &sUcHardwareId,
                                    TRUE))          // TRUE: ignore case
        // DB-Soc1-PCIe board
    {
        eResult = ePNDEV_BOARD_DB_SOC1_PCIE;

        // set TraceDetail
        RtlStringCchPrintfA(sTraceDetail,
                            _countof(sTraceDetail),
                            "PnDev[x,x,x]: ++++ DB-Soc1-PCIe board");

        // set trace
        fnSetTrace( ePNDEV_TRACE_LEVEL_INFO,
                    ePNDEV_TRACE_CONTEXT_BASIC,
                    TRUE,                       // logging
                    sTraceDetail);
    }
    else if (RtlPrefixUnicodeString(&sUcHardwareId_Fpga1_Ertec200P,
                                    &sUcHardwareId,
                                    TRUE))          // TRUE: ignore case
        // FPGA1-Ertec200P board
    {
        eResult = ePNDEV_BOARD_FPGA1_ERTEC200P;

        // set TraceDetail
        RtlStringCchPrintfA(sTraceDetail,
                            _countof(sTraceDetail),
                            "PnDev[x,x,x]: ++++ FPGA1-Ertec200P board");

        // set trace
        fnSetTrace( ePNDEV_TRACE_LEVEL_INFO,
                    ePNDEV_TRACE_CONTEXT_BASIC,
                    TRUE,                       // logging
                    sTraceDetail);
    }
    else if (RtlPrefixUnicodeString(&sUcHardwareId_Eb200P,
                                    &sUcHardwareId,
                                    TRUE))          // TRUE: ignore case
        // EB200P board
    {
        eResult = ePNDEV_BOARD_EB200P;

        // set TraceDetail
        RtlStringCchPrintfA(sTraceDetail,
                            _countof(sTraceDetail),
                            "PnDev[x,x,x]: ++++ EB200P board");

        // set trace
        fnSetTrace( ePNDEV_TRACE_LEVEL_INFO,
                    ePNDEV_TRACE_CONTEXT_BASIC,
                    TRUE,                       // logging
                    sTraceDetail);
    }
    else if (RtlPrefixUnicodeString(&sUcHardwareId_I210,
                                    &sUcHardwareId,
                                    TRUE))          // TRUE: ignore case
        // I210 board
    {
        eResult = ePNDEV_BOARD_I210;

        // set TraceDetail
        RtlStringCchPrintfA(sTraceDetail,
                            _countof(sTraceDetail),
                            "PnDev[x,x,x]: ++++ Intel I210 board");

        // set trace
		fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
					ePNDEV_TRACE_CONTEXT_BASIC,
					TRUE,						// logging
					sTraceDetail);
    }
	else if	(RtlPrefixUnicodeString(&sUcHardwareId_I82574,
									&sUcHardwareId,
									TRUE))			// TRUE: ignore case
		// Intel I82574 board
	{
		eResult = ePNDEV_BOARD_I82574;

		// set TraceDetail
		RtlStringCchPrintfA(sTraceDetail,
							_countof(sTraceDetail),
							"PnDev[x,x,x]: ++++ Intel I82574 board");

		 // set trace
		fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
					ePNDEV_TRACE_CONTEXT_BASIC,
					TRUE,						// logging
					sTraceDetail);
	}
	else if	(RtlPrefixUnicodeString(&sUcHardwareId_Fpga1_Hera,
									&sUcHardwareId,
									TRUE))			// TRUE: ignore case
		// FPGA1-Hera board
	{
		eResult = ePNDEV_BOARD_FPGA1_HERA;

		// set TraceDetail
		RtlStringCchPrintfA(sTraceDetail,
							_countof(sTraceDetail),
							"PnDev[x,x,x]: ++++ FPGA1-Hera board");

		 // set trace
		fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
					ePNDEV_TRACE_CONTEXT_BASIC,
					TRUE,						// logging
					sTraceDetail);
	}
	else if	(RtlPrefixUnicodeString(&sUcHardwareId_Micrel_KSZ8841,
									&sUcHardwareId,
									TRUE))			// TRUE: ignore case
		// Micrel KSZ8842 board
	{
		eResult = ePNDEV_BOARD_MICREL_KSZ8841;

		// set TraceDetail
		RtlStringCchPrintfA(sTraceDetail,
							_countof(sTraceDetail),
							"PnDev[x,x,x]: ++++ Micrel KSZ8841 board");

		 // set trace
		fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
					ePNDEV_TRACE_CONTEXT_BASIC,
					TRUE,						// logging
					sTraceDetail);
	}
	else if	(RtlPrefixUnicodeString(&sUcHardwareId_Micrel_KSZ8842,
									&sUcHardwareId,
									TRUE))			// TRUE: ignore case
		// Micrel KSZ8842 board
	{
		eResult = ePNDEV_BOARD_MICREL_KSZ8842;

		// set TraceDetail
		RtlStringCchPrintfA(sTraceDetail,
							_countof(sTraceDetail),
							"PnDev[x,x,x]: ++++ Micrel KSZ8842 board");

		 // set trace
		fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
					ePNDEV_TRACE_CONTEXT_BASIC,
					TRUE,						// logging
					sTraceDetail);
	}
	else if	(RtlPrefixUnicodeString(&sUcHardwareId_Ti_Omap,
									&sUcHardwareId,
									TRUE))			// TRUE: ignore case
		// Micrel KSZ8842 board
	{
		eResult = ePNDEV_BOARD_TI_OMAP;

		// set TraceDetail
		RtlStringCchPrintfA(sTraceDetail,
							_countof(sTraceDetail),
							"PnDev[x,x,x]: ++++ TI Omap Eval-board");

		 // set trace
		fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
					ePNDEV_TRACE_CONTEXT_BASIC,
					TRUE,						// logging
					sTraceDetail);
	}
    else
    {
        // set TraceDetail
        RtlStringCchPrintfA(sTraceDetail,
                            _countof(sTraceDetail),
                            "PnDev[x,x,x]: ########## Invalid board!");

        // set trace
        fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                    ePNDEV_TRACE_CONTEXT_BASIC,
                    TRUE,                       // logging
                    sTraceDetail);
    }
    

    return(eResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
ePNDEV_BOARD fnGetBoardType(const _TCHAR* sHardwareIdIn)
{
ePNDEV_BOARD eResult = ePNDEV_BOARD_INVALID;

    return(eResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  get first free index of EventHandleTable
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
BOOLEAN fnGetFreeIndexEventHandleTable( const UINT32                lDimArrayIn,
                                        uPNDEV_EVENT_HANDLE_TABLE   uArrayEventHandleTableIn[],
                                        const UINT32                lDimErrorIn,
                                        UINT32*                     pIndexOut,
                                        _TCHAR*                     sErrorOut)
{
BOOLEAN bResult = FALSE;
UINT32  i       = 0;

    // preset OutputParameter
    {
        *pIndexOut = MAXUINT32;

        _tcscpy_s(  sErrorOut,
                    lDimErrorIn,
                    _TEXT(""));
    }

    for (i = 0; i < lDimArrayIn; i++)
        // all entries of UsedDeviceList
    {
    uPNDEV_EVENT_HANDLE_TABLE* pEntryTmp = NULL;

        pEntryTmp = &uArrayEventHandleTableIn[i];

        if  (pEntryTmp->pEventObject == NULL)
            // free entry
        {
            // success
            bResult = TRUE;

            // return
            *pIndexOut = i;

            // leave loop
            break;
        }
    }

    if  (!bResult)
        // error
    {
        // set ErrorString
        _tcscpy_s(  sErrorOut, lDimErrorIn, _TEXT(__FUNCTION__));
        _tcsncat_s( sErrorOut, lDimErrorIn, _TEXT("(): No free entry!"), _TRUNCATE);
    }

    return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  release used index of EventHandleTable
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
BOOLEAN fnReleaseIndexEventHandleTable( const UINT32                lIndexIn,
                                        const UINT32                lDimArrayIn,
                                        uPNDEV_EVENT_HANDLE_TABLE   uArrayEventHandleTableIn[],
                                        const UINT32                lDimErrorIn,
                                        _TCHAR*                     sErrorOut)
{
BOOLEAN bResult = FALSE;

    if  (lIndexIn >= lDimArrayIn)
        // invalid index
    {
        // set ErrorString
        _tcscpy_s(  sErrorOut, lDimErrorIn, _TEXT(__FUNCTION__));
        _tcsncat_s( sErrorOut, lDimErrorIn, _TEXT("(): Invalid index!"), _TRUNCATE);
    }
    else
    {
        // success
        bResult = TRUE;

        memset( &uArrayEventHandleTableIn[lIndexIn],
                0,
                sizeof(uPNDEV_EVENT_HANDLE_TABLE));
    }

    return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif


//************************************************************************
//  D e s c r i p t i o n :
//
//  allocate physical contiguous memory for the DMA of the boards.
//  lAdrMin and lAdrMax is determined by the address window that the boards can access (determined by PCI/e Bridge!)
//  This function has a high chance of success if it is called at DriverEntry.
//  Due to memory fragmentaion it is more likely that the function will fail if it is called at a later time (e.g. AddDevice, OpenDevice...).
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
void fnMmAllocateContiguousMemory(  UINT32                          lSizeIn,
                                    CONST UINT32                    lAdrMinIn,
                                    CONST UINT32                    lAdrMaxIn,
                                    uPNDEV_HW_DESC_CONT_MEM_HOST*   pContMemOut)
{
void*               pAdr = NULL;
PHYSICAL_ADDRESS    uAdrMin;
PHYSICAL_ADDRESS    uAdrMax;
PHYSICAL_ADDRESS    uAdrBound;
PHYSICAL_ADDRESS    uPhyAdr;
char                sTraceDetail[PNDEV_SIZE_STRING_BUF] = {0};

    // set
    uAdrMin.u.HighPart  = 0UL;
    uAdrMin.u.LowPart   = lAdrMinIn;

    uAdrMax.u.HighPart  = 0UL;
    uAdrMax.u.LowPart   = lAdrMaxIn;

    // not used
    uAdrBound.u.HighPart= 0UL;
    uAdrBound.u.LowPart = 0UL;

    // Allocate memory
    pAdr = MmAllocateContiguousMemorySpecifyCache(  lSizeIn,        // _In_      SIZE_T NumberOfBytes,
                                                    uAdrMin,        // _In_      PHYSICAL_ADDRESS LowestAcceptableAddress,
                                                    uAdrMax,        // _In_      PHYSICAL_ADDRESS HighestAcceptableAddress,
                                                    uAdrBound,      // _In_opt_  PHYSICAL_ADDRESS BoundaryAddressMultiple,
                                                    MmNonCached );  // _In_      MEMORY_CACHING_TYPE CacheType


    if(pAdr != NULL)
    {
    PHYSICAL_ADDRESS uPhyAdr                    = MmGetPhysicalAddress(pAdr);
    _TCHAR sErrorDetail[PNDEV_SIZE_STRING_BUF]  = {0};

        pContMemOut->lPhyAdr        = uPhyAdr.u.LowPart;
        pContMemOut->pAdrVirt       = pAdr;
        pContMemOut->lSize          = lSizeIn;

        RtlStringCchPrintfA(sTraceDetail,
                            _countof(sTraceDetail),
                            "PnDev - Allocated Contiguous Memory 0x%04x",
                            pContMemOut->lPhyAdr);

        // set trace
        fnSetTrace( ePNDEV_TRACE_LEVEL_INFO,
                    ePNDEV_TRACE_CONTEXT_BASIC,
                    TRUE,                       // logging
                    sTraceDetail);
    }
    else
    {
        pContMemOut->lPhyAdr        = 0;
        pContMemOut->pAdrVirt       = NULL;
        pContMemOut->lSize          = 0;

        RtlStringCchPrintfA(sTraceDetail,
                            _countof(sTraceDetail),
                            "PnDev - FAILED to allocate Contiguous Memory");

        // set trace
        fnSetTrace( ePNDEV_TRACE_LEVEL_INFO,
                    ePNDEV_TRACE_CONTEXT_BASIC,
                    TRUE,                       // logging
                    sTraceDetail);
    }

    pContMemOut->bInUse = FALSE;
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
void fnMmAllocateContiguousMemory(  UINT32                          lSizeIn,
                                    const UINT32                    lAdrMinIn,
                                    const UINT32                    lAdrMaxIn,
                                    uPNDEV_HW_DESC_CONT_MEM_HOST*   pContMemOut)
{
UINT8*  pTemp                               = NULL;
char    sTraceDetail[PNDEV_SIZE_STRING_BUF] = {0};

    // allocate Mem;
    pTemp = (UINT8*) malloc(lSizeIn);

    if(pTemp != NULL)
    {
    _TCHAR sErrorDetail[PNDEV_SIZE_STRING_BUF]  = {0};

        pContMemOut->lPhyAdr        = adn_mem_mmu_translate((UINT32) pTemp);
        pContMemOut->pAdrVirt       = pTemp;
        pContMemOut->lSize          = lSizeIn;

        RtlStringCchPrintfA(sTraceDetail,
                            _countof(sTraceDetail),
                            "PnDev - Allocated Contiguous Memory 0x%04x",
                            pContMemOut->lPhyAdr);

        // set trace
        fnSetTrace( ePNDEV_TRACE_LEVEL_INFO,
                    ePNDEV_TRACE_CONTEXT_BASIC,
                    TRUE,                       // logging
                    sTraceDetail);
    }
    else
    {
        pContMemOut->lPhyAdr  = 0;
        pContMemOut->pAdrVirt = NULL;
        pContMemOut->lSize    = 0;

        RtlStringCchPrintfA(sTraceDetail,
                            _countof(sTraceDetail),
                            "PnDev - FAILED to allocate Contiguous Memory");

        // set trace
        fnSetTrace( ePNDEV_TRACE_LEVEL_INFO,
                    ePNDEV_TRACE_CONTEXT_BASIC,
                    TRUE,                       // logging
                    sTraceDetail);
    }

    pContMemOut->bInUse = FALSE;

}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------

#define PNDEVDRV_DMA_ALLOC_SIZE     (16 * 1024 * 1024)                                                  /* 16 MB */
#define DMA_ALLOC_MAX_DUMP_DEFAULT  2500                                                                /* entries for none fitting blocks */
#define DMA_MAX_ALLOC_BLOCK_SIZE    (4 * 1024 * 1024)                                                   /* 4 MB */
#define DMA_ALLOC_MAX_BUFFER        ((PNDEVDRV_DMA_ALLOC_SIZE / DMA_MAX_ALLOC_BLOCK_SIZE) * 2 - 1)      /* entries for allocated contigious memory */ 

void fnMmAllocateContiguousMemory(  UINT32                          lSizeIn,
                                    const UINT32                    lAdrMinIn,
                                    const UINT32                    lAdrMaxIn,
                                    uPNDEV_HW_DESC_CONT_MEM_HOST*   pContMemOut)
{
char        sTraceDetail[PNDEV_SIZE_STRING_BUF] = {0};
char**      pDumpVirt                           = NULL;
dma_addr_t* uDumpPhys                           = NULL;
INT32       uDumpIndex                          = 0;
UINT64      uDumpArrayCount = 0;


    if (lSizeIn == PNDEVDRV_DMA_ALLOC_SIZE /* 16MB or 8 MB*/)
        /* split in smaller alloc sections (max. alloc limit is DMA_MAX_ALLOC_BLOCK_SIZE)
         *
         * algorithm goes like this:
         * - array pBufVirt: 2 * blocks needed (contains virt address of physical aligned blocks, when finished)
         * - array pBufPhys: contains phy adress of allocated blocks (just for saving physical addresses)
         *
         * Step 1: - allocate a 4MB (DMA_MAX_ALLOC_BLOCK_SIZE) block and save in the middle of the array (count == 0)
         * Step 2: - allocate next 4MB block, and place in arrays (pBufVirt and pBufPhys) at
         *         -- index + 1, if adr is bigger then the adr of the last allocated block
         *         -- index - 1, if adr is smaller then the adr of the last allocated block
         * Step 3: - check if array pBufVirt is BOTTOM (index == 0) or TOP (index == DMA_ALLOC_MAX_BUFFER)
         *         -- BOTTOM: return virt and phys adr at index = 0 (blocks are descending ordered: last block allocated has return adr)
         *         -- TOP: return virt and phys adr at index = (DMA_ALLOC_MAX_BUFFER/2), what is the first block allocated. (blocks are ascending ordered)
         *         -- not BOTTOM nor TOP: goTo Step 2 and allocate next block
         */
    {
    struct file*    pFile;
    loff_t          pos             = 0;
    mm_segment_t    old_fs;
    char            pLine[256];

    char**          pBufVirt        = NULL;
    dma_addr_t*     uBufPhys        = NULL;
    UINT8           init            = 1;
    UINT16          index           = DMA_ALLOC_MAX_BUFFER / 2;

    
        /* Read size of  */
        old_fs = get_fs();  /* save current FS segment before reading */
        set_fs(get_ds());
    
        pFile = filp_open("/proc/meminfo", O_RDONLY, 0);

        if(pFile == NULL)
            /* error, could not open file */
        {
            uDumpArrayCount = DMA_ALLOC_MAX_DUMP_DEFAULT;    /* default value, works on Systems with less than 16GB of RAM */
            
            RtlStringCchPrintfA(sTraceDetail,
                                _countof(sTraceDetail),
                                "PnDev[x,x,x]: WARNING could not get real size of RAM. Take default value for bufferlist: %llu entries",
                                uDumpArrayCount);

            /* set trace */
            fnSetTrace( ePNDEV_TRACE_LEVEL_INFO,
                        ePNDEV_TRACE_CONTEXT_BASIC,
                        TRUE,                       /* logging */
                        sTraceDetail);
        }
        else
            /* Read size of installed RAM out of file */
        {
            if(vfs_read(pFile, pLine, sizeof(pLine), &pos))
            {
            UINT64  uRAM;
                if(sscanf(pLine, "MemTotal: %llu kB", &uRAM) == 1)
                {
                    uDumpArrayCount =  (uRAM * 1024 /* Bytes */) / DMA_MAX_ALLOC_BLOCK_SIZE; //Number of max amount of not fittig blocks to store in dumplist
                }
            }
            filp_close(pFile,NULL);  /* close file */
        
            set_fs(old_fs); /* Reset stored FS */

//            /* Info */
//            RtlStringCchPrintfA(sTraceDetail,
//                                _countof(sTraceDetail),
//                                "PnDev[x,x,x]: Found %llukB of RAM: %llu entries for bufferlist",
//                                (uDumpArrayCount * DMA_MAX_ALLOC_BLOCK_SIZE /* Bytes */) / 1024,
//                                uDumpArrayCount);
//
//            /* set trace */
//            fnSetTrace( ePNDEV_TRACE_LEVEL_INFO,
//                        ePNDEV_TRACE_CONTEXT_BASIC,
//                        TRUE,                       /* logging */
//                        sTraceDetail);
        }

        /* buffers for saving none fitting blocks */
        pDumpVirt = (char**) vmalloc(sizeof(char*) * uDumpArrayCount );
        uDumpPhys = (dma_addr_t*) vmalloc(sizeof(dma_addr_t) * uDumpArrayCount );
        
        /* buffer for fitting blocks */
        pBufVirt = (char**) vmalloc(sizeof(char*) * DMA_ALLOC_MAX_BUFFER);
        uBufPhys = (dma_addr_t*) vmalloc(sizeof(dma_addr_t) * DMA_ALLOC_MAX_BUFFER);
            
        if(pDumpVirt == NULL || uDumpPhys == NULL || pBufVirt == NULL || uBufPhys == NULL)
            /* error - pDumpVirt or uDumpPhys, pBufVirt or uBufPhys couldn't be allocated */
        {
            return;
        }
        else
            /* null the memory */
        {
            int i;

            for(i = 0; i < uDumpArrayCount; i++)
            {
                pDumpVirt[i] = NULL;
            }
            
            for(i = 0; i < DMA_ALLOC_MAX_BUFFER; i++)
            {
                pBufVirt[i] = NULL;
                
            }
         }

        /* start with the algorithm */
        while(index >= 0 && index < DMA_ALLOC_MAX_BUFFER)
        {
        char*       pAddrVirt;
        dma_addr_t  uAddrPhys;

            /* allocate one block (4MB) in whole kernel space */
            pAddrVirt = kmalloc(DMA_MAX_ALLOC_BLOCK_SIZE, GFP_KERNEL);
            uAddrPhys = virt_to_phys(pAddrVirt);
        
            if(pAddrVirt == NULL)
                /* error - not enough RAM for allocation of 4MB (DMA_MAX_ALLOC_BLOCK_SIZE) Block*/
            {
                RtlStringCchPrintfA(sTraceDetail,
                                    _countof(sTraceDetail),
                                    "PnDev[x,x,x]: FAILED to allocate Contiguous Memory - could not allocate block of size=%dB",
                                    DMA_MAX_ALLOC_BLOCK_SIZE);

                /* set trace */
                fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                            ePNDEV_TRACE_CONTEXT_BASIC,
                            TRUE,                       /* logging */
                            sTraceDetail);

                for(index = 0; index < DMA_ALLOC_MAX_BUFFER; index++)
                {
                    if(pBufVirt[index] != NULL)
                    {
                        /* free all saved buffers and exit*/
                        kfree(pBufVirt[index]);
                    }
                }

                break;
            }
            else if (lAdrMaxIn > 0 && (UINT64)uAddrPhys >= (UINT64) (lAdrMaxIn - DMA_MAX_ALLOC_BLOCK_SIZE))
                /* error - memory of allocated block is above max address (lAdrMaxIn) */
            {
                if(uDumpIndex >= uDumpArrayCount)
                    /* error - not enough space in pDumpVirt and uDumpPhys array - Free and EXIT*/
                {
                    RtlStringCchPrintfA(sTraceDetail,
                                        _countof(sTraceDetail),
                                        "PnDev[x,x,x]: FAILED to allocate Contiguous Memory - not enough space to save overaddressed memory blocks");

                    /* set trace */
                    fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                                ePNDEV_TRACE_CONTEXT_BASIC,
                                TRUE,                       /* logging */
                                sTraceDetail);

                    for(index = 0; index < DMA_ALLOC_MAX_BUFFER; index++)
                    {
                        if(pBufVirt[index] != NULL)
                        {
                            /* free saved blocks, if there are some */
                            kfree(pBufVirt[index]);
                        }
                    }

                    break;
                }
                else
                    /* put block in dummybufferlist and get next block */
                {
                    pDumpVirt[uDumpIndex] = pAddrVirt;
                    uDumpPhys[uDumpIndex] = uAddrPhys;
                    uDumpIndex++;

                    continue;
                }
            }
            else if (init == 1)
                /* first block is initialization */
            {
                pBufVirt[index] = pAddrVirt;
                uBufPhys[index] = uAddrPhys;
                init = 0;
                continue;
            }
            else
                /* successfully allocated */
            {
                if((UINT64)uBufPhys[index] - (UINT64)uAddrPhys == (UINT64)DMA_MAX_ALLOC_BLOCK_SIZE)
                    /* success - allocated buffer is below last allocated buffer */
                {
                    if(index == 0)
                        /* error - buffer full */
                    {
                        break;
                    }
                    else
                        /* success - store next blockaddress in array */
                    {
                        index = index - 1;
                        pBufVirt[index] = pAddrVirt;
                        uBufPhys[index] = uAddrPhys;
                    }

                }
                else if((UINT64)uAddrPhys - (UINT64)uBufPhys[index] == (UINT64)DMA_MAX_ALLOC_BLOCK_SIZE)
                    /* success - allocated buffer is above last allocated buffer */
                {
                    if(index > (DMA_ALLOC_MAX_BUFFER - 1) )
                        /* error - buffer full */
                    {
                        break;
                    }
                    else
                        /* success - store next blockaddress in array */
                    {
                        index = index + 1;
                        pBufVirt[index] = pAddrVirt;
                        uBufPhys[index] = uAddrPhys;
                    }

                }
                else
                    /* error - allocated block is not directly fitting to block allocated bevor: memory not physically contiguous */
                {
                UINT8   i;   
                    for(i = 0; i < DMA_ALLOC_MAX_BUFFER; i++)
                        /* move so far allocated blocks in dump list - will be freed at end of function*/
                    {
                        if(pBufVirt[i] != NULL)
                        {
                            if(uDumpIndex >= uDumpArrayCount)
                                /* error - not enough space in pDumpVirt and uDumpPhys array */
                            {
                                RtlStringCchPrintfA(sTraceDetail,
                                                    _countof(sTraceDetail),
                                                    "PnDev[x,x,x]: FAILED to allocate Contiguous Memory - not enough space to save incontiguous memory blocks");

                                /* set trace */
                                fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                                            ePNDEV_TRACE_CONTEXT_BASIC,
                                            TRUE,                       /* logging */
                                            sTraceDetail);

                                for(index = 0; index < DMA_ALLOC_MAX_BUFFER; index++)
                                    /* free allocated blocks in buffer list */
                                {
                                    if(pBufVirt[index] != NULL)
                                    {
                                        /* free saved blocks */
                                        kfree(pBufVirt[index]);
                                    }
                                }

                                break;
                            }
                            else
                                /* success - move block */
                            {
                                pDumpVirt[uDumpIndex] = pBufVirt[i];
                                uDumpPhys[uDumpIndex] = uBufPhys[i];
                                pBufVirt[i] = NULL; /* set buffer empty */
                                uDumpIndex++;
                            }
                        }
                    }
                    
                    /* start algorithm with new startblock */
                    index = DMA_ALLOC_MAX_BUFFER / 2;
                    pBufVirt[index] = pAddrVirt;
                    uBufPhys[index] = uAddrPhys;

                    continue;
                }
            }

            /* check if 4 blocks were allocated and 16MB are reached*/
            if(index == 0)
                /* got descending memory, return last allocated block */
            {
                pContMemOut->lPhyAdr = uBufPhys[0];
                pContMemOut->pAdrVirt = pBufVirt[0];
                pContMemOut->lSize = PNDEVDRV_DMA_ALLOC_SIZE;
            }
            else if(index == (DMA_ALLOC_MAX_BUFFER - 1))
                /* got ascending memory, return first allocated block */
            {
                pContMemOut->lPhyAdr    = uBufPhys[DMA_ALLOC_MAX_BUFFER / 2];
                pContMemOut->pAdrVirt   = pBufVirt[DMA_ALLOC_MAX_BUFFER / 2];
                pContMemOut->lSize      = PNDEVDRV_DMA_ALLOC_SIZE; /* 16MB */
            }
            
            /* Output success */
            if(index == 0  || index == (DMA_ALLOC_MAX_BUFFER - 1))
            {
                RtlStringCchPrintfA(sTraceDetail,
                                    _countof(sTraceDetail),
                                    "PnDev[x,x,x]: allocated contiguous memory: virt=0x%p, phys=0x%x, size=%dB",
                                    pContMemOut->pAdrVirt,
                                    pContMemOut->lPhyAdr,
                                    pContMemOut->lSize);

                /* set trace */
                fnSetTrace( ePNDEV_TRACE_LEVEL_INFO,
                            ePNDEV_TRACE_CONTEXT_BASIC,
                            TRUE,                       /* logging */
                            sTraceDetail);

                /* all done - quit while loop*/
                break;
            }
        } /* while loop */
        
        /* Free lists with memory addresses */
        vfree(pBufVirt);
        vfree(uBufPhys);
    }
    else
        /* alloc memory until 4MB and exit */
    {
    char*       pVirt;
    dma_addr_t  uPhys;

        pVirt = kmalloc(lSizeIn, GFP_KERNEL);
        uPhys = virt_to_phys(pVirt);

        if(pVirt == NULL)
            /* Not enough space for allocation of lSizeIn Kbytes */
        {
            RtlStringCchPrintfA(sTraceDetail,
                                _countof(sTraceDetail),
                                "PnDev - FAILED to allocate Contiguous Memory - not enough memory");

            /* set trace */
            fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                        ePNDEV_TRACE_CONTEXT_BASIC,
                        TRUE,                       /* logging */
                        sTraceDetail);
        }
        else
        {
            pContMemOut->pAdrVirt = pVirt;
            pContMemOut->lPhyAdr = uPhys;
            pContMemOut->lSize = lSizeIn;
            
            RtlStringCchPrintfA(sTraceDetail,
                                _countof(sTraceDetail),
                                "PnDev[x,x,x]: allocated contiguous memory: virt=0x%p, phys=0x%x, size=%dB",
                                pContMemOut->pAdrVirt,
                                pContMemOut->lPhyAdr,
                                pContMemOut->lSize);

            /* set trace */
            fnSetTrace( ePNDEV_TRACE_LEVEL_INFO,
                        ePNDEV_TRACE_CONTEXT_BASIC,
                        TRUE,                       /* logging */
                        sTraceDetail);
        }
    }

    /* Free list of dummybuffers if they were used */
    for (uDumpIndex--; uDumpIndex >= 0 && uDumpIndex < uDumpArrayCount ; uDumpIndex--)
    {
        if(pDumpVirt[uDumpIndex] != NULL)
        {
            kfree(pDumpVirt[uDumpIndex]);
        }
    }
    
    /* free the save lists */
    vfree(pDumpVirt);
    vfree(uDumpPhys);
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  Frees the memory
//************************************************************************
void fnMmFreeContiguousMemory(uPNDEV_HW_DESC_CONT_MEM_HOST *pContMemIn)
{
    if(pContMemIn->pAdrVirt != NULL)
    {
        // MEMORY_CACHING_TYPE CacheType and SIZE_T NumberOfBytes must match with the parameter passed at MmAllocateContiguousMemorySpecifyCache (see fnMmAllocateContiguousMemory)
        MmFreeContiguousMemorySpecifyCache( (PVOID) pContMemIn->pAdrVirt,
                                            pContMemIn->lSize,
                                            MmNonCached);

        pContMemIn->bInUse     = FALSE;
        pContMemIn->lPhyAdr    = 0;
        pContMemIn->pAdrVirt   = NULL;
        pContMemIn->lSize      = 0;
    }
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
void fnMmFreeContiguousMemory(uPNDEV_HW_DESC_CONT_MEM_HOST *pContMemIn)
{
    if(pContMemIn->pAdrVirt != NULL)
    {
        free((void*) pContMemIn->pAdrVirt);

        pContMemIn->bInUse     = FALSE;
        pContMemIn->lPhyAdr    = 0;
        pContMemIn->pAdrVirt   = NULL;
        pContMemIn->lSize      = 0;
    }
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
void fnMmFreeContiguousMemory(uPNDEV_HW_DESC_CONT_MEM_HOST *pContMemIn)
{
    if(pContMemIn->pAdrVirt != NULL)
    {
    char    sTraceDetail[PNDEV_SIZE_STRING_BUF] = {0};
    
        if(pContMemIn->lSize == PNDEVDRV_DMA_ALLOC_SIZE)
            /* free the 16MB buffer in 4 steps a 4MB*/
        {
            kfree((void *) (pContMemIn->pAdrVirt + 3 * DMA_MAX_ALLOC_BLOCK_SIZE));
            kfree((void *) (pContMemIn->pAdrVirt + 2 * DMA_MAX_ALLOC_BLOCK_SIZE));
            kfree((void *) (pContMemIn->pAdrVirt + DMA_MAX_ALLOC_BLOCK_SIZE));
            kfree((void *) pContMemIn->pAdrVirt);
        }
        else
            /* free the buffer*/
        {
            kfree((void *) pContMemIn->pAdrVirt);
        }
        
            RtlStringCchPrintfA(sTraceDetail,
                                _countof(sTraceDetail),
                                "PnDev[x,x,x]: successfully freed contiguous memory: virt=0x%p, phys=0x%x, size=%dB",
                                pContMemIn->pAdrVirt,
                                pContMemIn->lPhyAdr,
                                pContMemIn->lSize);
    
            /* set trace */
            fnSetTrace( ePNDEV_TRACE_LEVEL_INFO,
                        ePNDEV_TRACE_CONTEXT_BASIC,
                        TRUE,                           /* logging */
                        sTraceDetail);
        
        /* reset host struct */
        pContMemIn->bInUse     = FALSE;
        pContMemIn->lPhyAdr    = 0;
        pContMemIn->pAdrVirt   = NULL;
        pContMemIn->lSize      = 0;
    }
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  OS abstraction for NetProfi Kernel Interrupt handling
//************************************************************************
//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN fnNetProfiIntHandler(DEVICE_EXTENSION* pDevExtIn)
{
    // wrapper for NetProfi Kernel
    return fnXx_IntHandler(pDevExtIn);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
BOOLEAN fnNetProfiIntHandler(   DEVICE_EXTENSION* pDevExtIn)
{
    // not supported in Adonis
    return FALSE;
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN fnNetProfiIntHandler(   DEVICE_EXTENSION* pDevExtIn)
{
    // not supported in Linux
    return FALSE;
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  Linux FunctionOption .open
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
int fnOpenDriver(   struct inode *inode,
                    struct file *file)
{
UINT32 lResult = LINUX_ERROR;

    if  (try_module_get(THIS_MODULE))
        // got module reference
    {
        lResult = LINUX_OK;
    }

    return lResult;
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  Linux FunctionOption .release
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
int fnCloseDriver(  struct inode *inode,
                    struct file *file)
{
    // release module reference
    module_put(THIS_MODULE);

    return LINUX_OK;
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  Linux RetrieveIoctlBuffer
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN RetrieveIoctlBuffer(unsigned int        lCmdIn,
                            unsigned long       lArgIn,
                            uPNDEV_IOCTL_ARG*   pIoctlArgIn,
                            uPNDEV64_PTR_VOID*  pInBufferIn,
                            uPNDEV64_PTR_VOID*  pOutBufferIn)
{
BOOLEAN bResult         = FALSE;
BOOLEAN bResultIoctl    = FALSE;
INT32   lResultTmp      = LINUX_ERROR;
char    sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};


    // check input arguments
    {
        if  (_IOC_TYPE(lCmdIn) != PNDEV_IOC_MAGIC)
            // no valid MagicNumber included
        {
            // set TraceDetail
            RtlStringCchPrintfA(sTraceDetail,
                                _countof(sTraceDetail),
                                "PnDev[x,x,x]: ########## Invalid MagicNumber in IoctlCmd!");

            // set trace
            fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                        ePNDEV_TRACE_CONTEXT_BASIC,
                        TRUE,                       // logging
                        sTraceDetail);

        }
        else
        {
            if  (_IOC_DIR(lCmdIn) & _IOC_READ)
                // Ioctl has ReadDirection
            {
                // check if Useradress is writable
                lResultTmp = access_ok( VERIFY_WRITE,
                                        (void __user *) lArgIn,
                                        sizeof(uPNDEV_IOCTL_ARG));

                if  (   (_IOC_DIR(lCmdIn) & _IOC_WRITE)
                    &&  (lResultTmp > 0))
                    // Ioctl has WriteDirection
                {
                    lResultTmp = access_ok( VERIFY_READ,
                                            (void __user *) lArgIn,
                                            sizeof(uPNDEV_IOCTL_ARG));

                    if  (lResultTmp > 0)
                        // success
                    {
                        bResultIoctl = TRUE;
                    }
                    else
                    {
                        // set TraceDetail
                        RtlStringCchPrintfA(sTraceDetail,
                                            _countof(sTraceDetail),
                                            "PnDev[x,x,x]: ########## Error at access_ok()!");

                        // set trace
                        fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                                    ePNDEV_TRACE_CONTEXT_BASIC,
                                    TRUE,                       // logging
                                    sTraceDetail);
                    }
                }
            }
        }
    }

    if  (!bResultIoctl)
        // error
    {
        // set TraceDetail
        RtlStringCchPrintfA(sTraceDetail,
                            _countof(sTraceDetail),
                            "PnDev[x,x,x]: ########## RetrieveIoctlBuffer access error on buffer");

        // set trace
        fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                    ePNDEV_TRACE_CONTEXT_BASIC,
                    TRUE,                       // logging
                    sTraceDetail);

    }
    else
    {
        // get IoctlArg
        lResultTmp = copy_from_user((void*) pIoctlArgIn,
                                    (void __user *) lArgIn,
                                    sizeof(uPNDEV_IOCTL_ARG));

        if  (lResultTmp > 0)
            // error at copy data
        {
            // set TraceDetail
            RtlStringCchPrintfA(sTraceDetail,
                                _countof(sTraceDetail),
                                "PnDev[x,x,x]: ########## Error at copy_from_user!");

            // set trace
            fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                        ePNDEV_TRACE_CONTEXT_BASIC,
                        TRUE,                       // logging
                        sTraceDetail);
        }
        else
        {
            // alloc InBuffer
            pInBufferIn->pPtr = vmalloc(pIoctlArgIn->lSizeInputBuf);

            // alloc OutBuffer
            pOutBufferIn->pPtr = vmalloc(pIoctlArgIn->lSizeOutputBuf);

            if  (   (pInBufferIn->pPtr  == NULL)
                ||  (pOutBufferIn->pPtr == NULL))
                // no valid ptr
            {
                // set TraceDetail
                RtlStringCchPrintfA(sTraceDetail,
                                    _countof(sTraceDetail),
                                    "PnDev[x,x,x]: ########## Error at vmalloc for InBuffer oder OutBuffer!");

                // set trace
                fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                            ePNDEV_TRACE_CONTEXT_BASIC,
                            TRUE,                       // logging
                            sTraceDetail);
            }
            else
            {
                // get uIn
                lResultTmp = copy_from_user((void*) pInBufferIn->pPtr,
                                            (void __user *) pIoctlArgIn->uIn.pPtr,
                                            pIoctlArgIn->lSizeInputBuf);

                if  (lResultTmp > 0)
                    // error at copy data
                {
                    // set TraceDetail
                    RtlStringCchPrintfA(sTraceDetail,
                                        _countof(sTraceDetail),
                                        "PnDev[x,x,x]: ########## Error at copy_from_user InBuffer!");

                    // set trace
                    fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                                ePNDEV_TRACE_CONTEXT_BASIC,
                                TRUE,                       // logging
                                sTraceDetail);
                }
                else
                {
                    // get uOut
                    lResultTmp = copy_from_user((void*) pOutBufferIn->pPtr,
                                                (void __user *) pIoctlArgIn->uOut.pPtr,
                                                pIoctlArgIn->lSizeOutputBuf);

                    if  (lResultTmp > 0)
                        // error at copy data
                    {
                        // set TraceDetail
                        RtlStringCchPrintfA(sTraceDetail,
                                            _countof(sTraceDetail),
                                            "PnDev[x,x,x]: ########## Error at copy_from_user OutBuffer!");

                        // set trace
                        fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                                    ePNDEV_TRACE_CONTEXT_BASIC,
                                    TRUE,                       // logging
                                    sTraceDetail);
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

    return (bResult);
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  Linux ReturnIoctlBuffer
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN ReturnIoctlBuffer(  uPNDEV_IOCTL_ARG*   pIoctlArgIn,
                            uPNDEV64_PTR_VOID*  pInBufferIn,
                            uPNDEV64_PTR_VOID*  pOutBufferIn)
{
BOOLEAN bResult         = FALSE;
INT32   lResultTmp      = LINUX_ERROR;
char    sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

    if  (   (pInBufferIn  == NULL)
        ||  (pOutBufferIn == NULL))
        // no valid ptr
    {
        // set TraceDetail
        RtlStringCchPrintfA(sTraceDetail,
                            _countof(sTraceDetail),
                            "PnDev[x,x,x]: ########## No valid Ptr for InBuffer or OutBuffer!");

        // set trace
        fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                    ePNDEV_TRACE_CONTEXT_BASIC,
                    TRUE,                       // logging
                    sTraceDetail);
    }
    else
    {
        // get uIn
        lResultTmp = copy_to_user(  (void __user *) pIoctlArgIn->uIn.pPtr,
                                    (void*) pInBufferIn->pPtr,
                                    pIoctlArgIn->lSizeInputBuf);

        if  (lResultTmp > 0)
            // error at copy data
        {
            // set TraceDetail
            RtlStringCchPrintfA(sTraceDetail,
                                _countof(sTraceDetail),
                                "PnDev[x,x,x]: ########## Error at copy_to_user InBuffer!");

            // set trace
            fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                        ePNDEV_TRACE_CONTEXT_BASIC,
                        TRUE,                       // logging
                        sTraceDetail);
        }
        else
        {
            // get uOut
            lResultTmp = copy_to_user(  (void __user *) pIoctlArgIn->uOut.pPtr,
                                        (void*) pOutBufferIn->pPtr,
                                        pIoctlArgIn->lSizeOutputBuf);

            if  (lResultTmp > 0)
                // error at copy data
            {
                // set TraceDetail
                RtlStringCchPrintfA(sTraceDetail,
                                    _countof(sTraceDetail),
                                    "PnDev[x,x,x]: ########## Error at copy_to_user OutBuffer!");

                // set trace
                fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                            ePNDEV_TRACE_CONTEXT_BASIC,
                            TRUE,                       // logging
                            sTraceDetail);
            }
            else
            {
                // free InBuffer
                vfree(pInBufferIn->pPtr);

                // free OutBuffer
                vfree(pOutBufferIn->pPtr);

                // success
                bResult = TRUE;
            }
        }
    }

    return (bResult);
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  Linux Map Memory to Usermode
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
int fnEvtMmap(  struct file* pFdIn,
                struct vm_area_struct* pVmaIn)
{
INT32 lResult = LINUX_ERROR;

    if(io_remap_pfn_range(pVmaIn, pVmaIn->vm_start, pVmaIn->vm_pgoff, pVmaIn->vm_end - pVmaIn->vm_start, pVmaIn->vm_page_prot))
    {
        lResult = -EAGAIN;
    }
    else
    {
        lResult = LINUX_OK;
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
//  Callback functions for Linux
//  Check PciLoc in Array of UserInpput
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN fnCheckPciLocIn(PNDEV_CHAR*		sPciLocIn[],
						const UINT32	lDimArrayIn,
						UINT32			pBusIn,
						UINT32			pDevIn,
						UINT32			pFuncIn)
{
BOOLEAN	bResult		= FALSE;
BOOLEAN	bListEmpty 	= FALSE;
UINT32	i			= 0;
PNDEV_CHAR sPciLocInTmp[PNDEV_SIZE_STRING_BUF_SHORT] = {0};

	// set PciLocShort
	_RtlStringCchPrintf(sPciLocInTmp,
						_countof(sPciLocInTmp),
						_TEXT("%i/%i/%i"),
						pBusIn,
						pDevIn,
						pFuncIn);

	// preset
	bListEmpty = TRUE;

	// check if parameter list is empty
	for	(i = 0; i < lDimArrayIn; i++)
		// all entries of Array PciLocIn
	{
		if	(sPciLocIn[i] != NULL)
			// element not empty
		{
			bListEmpty = FALSE;

			break;
		}
	}

	if (bListEmpty)
		// List is empty
	{
		// success, because all boards should be added
		bResult = TRUE;
	}
	else
	{
		for	(i = 0; i < lDimArrayIn; i++)
			// all entries of Array PciLocIn
		{
			if (sPciLocIn[i] != NULL)

			{
				if	(_tcscmp(sPciLocInTmp, sPciLocIn[i]) == 0)
					// device found
				{
					// success
					bResult = TRUE;

					// leave loop
					break;
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
//************************************************************************
//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------

MODULE_LICENSE("GPL");

module_param_array(PciLoc, charp, &lPciLoc, 0000);
MODULE_PARM_DESC(PciLoc, "ParameterArray of PciLocations for insmod.");

module_init(DriverEntry);
module_exit(fnEvtDriverUnload);
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif
