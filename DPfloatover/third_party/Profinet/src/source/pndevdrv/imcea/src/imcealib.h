/*--FILE HEADER-(set tabulator to 4)-------------------------------------------
;
;   filename    : IMCEALIB.H
;   version     : 0001
;   contents    : Header Files to access the IMCEA-example 
;   author      : B.N.
;   date        : Dec 21, 2004
;
;---FUNCTION AND RMOS ENVIRONMENT----------------------------------------------
;
;   function    : Header Files to access the IMCEA-example
;
;   Windows
;
;---END OF FILE HEADER---------------------------------------------------------
;
;---MODIFICATION HEADER--------------------------------------------------------
;
;   Dec 21, 2004  M.J.  0001  first issue
;
;---END OF MODIFICATION HEADER-----------------------------------------------*/

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

VOID IMCEA_start(VOID);
DWORD ImcEADrvInstallDriver( char*, char* );
DWORD ImcEADrvUninstallDriver( char* );
DWORD ImcEADrvStartDriver( char* );
DWORD ImcEADrvStopDriver( char* );
DWORD ImcEADrvOpen( char*, HANDLE* );
DWORD ImcEADrvClose( HANDLE );
DWORD ImcEADrvGetVersion( HANDLE, PGETVERSION );
DWORD ImcEADrvReservePort( HANDLE, PRESERVEIO );
DWORD ImcEADrvFreePort( HANDLE, PRESERVEIO );
DWORD ImcEADrvWritePort( HANDLE, PPORTIO );
DWORD ImcEADrvReadPort( HANDLE, PPORTIO );
DWORD ImcEADrvReserveMem( HANDLE, PRESERVEIO );
DWORD ImcEADrvFreeMem( HANDLE, PRESERVEIO );
DWORD ImcEADrvStartInterruptHandler( HANDLE, PRESERVEINT );
DWORD ImcEADrvStopInterruptHandler( HANDLE, PRESERVEINT );
DWORD ImcEADrvWaitForInterrupt( HANDLE, PRESERVEINT );
DWORD ImcEADrvWaitForInterruptEx( HANDLE, PRESERVEINTEX );
DWORD ImcEADrvResetWaitForInterrupt( HANDLE, PRESERVEINT );
DWORD ImcEADrvSearchPCI( HANDLE hDrvHandle,
                         ULONG	VendorID,
                         ULONG	DeviceID,
                         ULONG	SubVendorID,
                         ULONG	SubDeviceID,
                         PCI_CONFIG_SPACE *ConfigSpace,
                         ULONG	MaxNumberOfFunctions,
                         ULONG *NumOfFunctions);
DWORD ImcEADrvGetPhysAdr( HANDLE hDrvHandle, PRESERVEPHY pReservePhy );
DWORD ImcEADrvFreePhysAdr( HANDLE hDrvHandle, PRESERVEPHY pReservePhy );
DWORD ImcEADrvDoIntr( HANDLE hDrvHandle, PDOINTERRUPT pDoInterrupt );
DWORD ImcEADrvReserveMemEx( HANDLE, PRESERVEIO );


