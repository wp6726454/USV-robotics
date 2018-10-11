#define  _CRT_SECURE_NO_DEPRECATE 1
/*--FILE HEADER-(set tabulator to 4)-------------------------------------------
;
;   filename    : IMCEALIB.C
;   version     : 0002
;   contents    : Header Files to access the IMCEA-example 
;   author      : B.N.
;   date        : Dec 21, 2004
;
;---FUNCTION AND RMOS ENVIRONMENT----------------------------------------------
;
;   function    : Source to access the IMCEA-Driver
;
;   Windows
;
;---END OF FILE HEADER---------------------------------------------------------
;
;---MODIFICATION HEADER--------------------------------------------------------
;
;   Dec 21, 2004  M.J.  0001  first issue
;   Jun 16, 2011  B.N.  0021  64-bit
;
;---END OF MODIFICATION HEADER-----------------------------------------------*/

#include <windows.h>
#include "imcdrv.h"			// Defines for the NT-Driver
#include "imcealib.h"		// Defines for our LIB

#include <winsvc.h>			// Defines for the SCManager
#include <winioctl.h>		// Defines for IOCTL


//****************************************************************************
//
//	Function:		ImcEADrvInstallDriver
//
//	Description:	See IMCEALIB.H
//
//****************************************************************************
DWORD ImcEADrvInstallDriver( char* szDriverPath, char* szDriverName )
{
	SC_HANDLE	hSCManager;			// Handle of the ServiceControlManager 
	SC_HANDLE	hService;			// Handle of the service
	BOOL		bRet;				// Value for a binary code
	DWORD		dwRetCode;			// Value for returncode

	dwRetCode = ERROR_SUCCESS;		// On the start all is OK

	// Open the SCManager 
	hSCManager = OpenSCManager( 
					NULL,					// local computer
					NULL,					// default database
					SC_MANAGER_ALL_ACCESS );// access to all

	if( hSCManager == NULL )
	{
		// error by the function OpenSCManager 
		// what error was it? Get it!
		dwRetCode = GetLastError();
	}
	else
	{
		// create a ServiceObject in the Database
		hService = CreateService(	
					hSCManager,			// handle to service control manager
					szDriverName,		// pointer to name of service to start
					szDriverName,		// pointer to display name; the same
					SERVICE_ALL_ACCESS,	// type of access to service
					SERVICE_KERNEL_DRIVER,	// type of service
					SERVICE_AUTO_START,	// when to start service
					SERVICE_ERROR_NORMAL,	// severity if service fails to start
					szDriverPath,		// pointer to name of binary file
					NULL, NULL, NULL, NULL, NULL );

		if( hService == NULL )
		{
			// error by the function CreateService
			// what error was it? Get it!
			dwRetCode = GetLastError();
		}
		else
		{
			// Close the Handle from Service 
			bRet = CloseServiceHandle( hService );

			if( bRet == FALSE )
			{
				// error by the function CloseService
				// what error was it? Get it!
				dwRetCode = GetLastError();
			}
			else
			{
				// Falls Treiber installiert wurde noch das Logging in
				// die Registry eintragen. 
				HKEY	hKey;
				LONG	lRet;
				DWORD	dwTmp = 0x07;
						
				// Hier nun das Eventlogging in die Registry eintragen!
				// Wir stellen uns in der Registery auf die Pfade
				// Fals der Key nicht existiert wird er angelegt.
				lRet = RegCreateKeyEx(	HKEY_LOCAL_MACHINE, 
										DRIVER_EA_LOG_REG_PATH, 
										0, NULL, 
										REG_OPTION_NON_VOLATILE, 
										KEY_ALL_ACCESS, 
										NULL, &hKey, NULL );

				if( lRet == ERROR_SUCCESS )
				{
					// Der Key wurde gefunden oder neu angelegt. 
					// Nun die Werte reinschreiben.
					lRet = RegSetValueEx(	hKey, 
											"EventMessageFile", 
											0, 
											REG_EXPAND_SZ,
											(const unsigned char*)szDriverPath,
											(DWORD)strlen((const char*)szDriverPath) );

					if( lRet == ERROR_SUCCESS )
					{
						lRet = RegSetValueEx(	hKey, 
												"TypesSupported",
												0, 
												REG_DWORD,
												(const unsigned char*)&dwTmp,
												sizeof( dwTmp ) );

					} // Ende RegValueSetEx

					RegCloseKey( hKey );		// Auf Fehler nicht mehr testen!

				} // Ende RegCreateKeyEx
			} // Ende else CloseServiceHandle 
		} // Ende else CreateService

		// Handle wieder freigeben
		CloseServiceHandle( hSCManager );

	} // SCManager 

    return dwRetCode;
}


//****************************************************************************
//
//	Function:		ImcEADrvUninstallDriver
//
//	Description:	See IMCEALIB.H
//
//****************************************************************************
DWORD ImcEADrvUninstallDriver( char* szDriverName )
{
	DWORD		dwRetCode;		// Value for returncode
	SC_HANDLE	hSCManager;		// Handle auf den ServiceControlManager 
	SC_HANDLE	hService;		// Handle fuer den Service
	BOOL		bRet;			// Variable nimmt BOOL auf
	LONG		lRet;
	HKEY		hKey;

	dwRetCode = ERROR_SUCCESS;		// Alles OK

	hSCManager = OpenSCManager( NULL,					// localer Rechner
								NULL,					// default Datenbank
								SC_MANAGER_ALL_ACCESS );// Zugriff auf alles

	if( hSCManager == NULL )
	{
		// Fehler bei der Funktion OpenSCManager aufgetreten
		// Nun schau mer mal welcher Fehler es den war?
		dwRetCode = GetLastError();
	}
	else
	{
		// Bis jetzt ist kein Fehler aufgetreten
		// Nun den Treiber Oeffnen
		hService = OpenService( hSCManager,
								szDriverName,
								SERVICE_ALL_ACCESS );

		if( hService == NULL )
		{
			dwRetCode = GetLastError();
		}
		else
		{
			// Bis hier war noch kein Fehler
			// Hier wird der Service geloescht
			bRet = DeleteService( hService );

			if( bRet == FALSE )
			{
				dwRetCode = GetLastError();
			}

			// Handle wieder zurueckgeben
			CloseServiceHandle( hService );

		} // Ende hService

		// Handle wieder zurueckgeben
		CloseServiceHandle( hSCManager );

	} // Ende else hSCManager

	// Nun die Meldungen aus der Registry wieder austragen
	// Auch wenn vorher Fehler aufgetreten sind

	lRet = RegOpenKeyEx(  
			HKEY_LOCAL_MACHINE,		// handle of open key
			DRIVER_EA_LOG_REG_PATH,	// address of name of subkey to open
			0,						// reserved  
			KEY_ALL_ACCESS,			// security access mask
			&hKey );				// address of handle of open key

	if( lRet == ERROR_SUCCESS )
	{
		// Registry-Eintrag loesschen
		lRet = RegDeleteKey(  
				HKEY_LOCAL_MACHINE, 		// handle of open key
				DRIVER_EA_LOG_REG_PATH );	// address of name of subkey to delete);

		// Wieder schliesen
		RegCloseKey( hKey );
	}

	if( dwRetCode == ERROR_SUCCESS )
	{
		// Treiber entfernt!
		// Nun schauen ob Registy OK
		if( lRet != ERROR_SUCCESS )
		{
			// Den anderen Fehlercode zurueck
			dwRetCode = lRet;
		}
	}

	return dwRetCode;
}


//****************************************************************************
// 
//	Function:		ImcEADrvStartDriver
//
//	Description:	See IMCEALIB.H
//
//****************************************************************************
DWORD ImcEADrvStartDriver( char* szDriverName )
{
	SC_HANDLE	hSCManager;		// Handle auf den ServiceControlManager 
	SC_HANDLE	hService;		// Handle fuer den Service
	DWORD		dwRetCode;		// Variable fuer Returncode
	BOOL		bRet;			// Variable nimmt BOOL auf

	dwRetCode = ERROR_SUCCESS;	// Am Anfang ist alles OK

	hSCManager = OpenSCManager( NULL,					// localer Rechner
								NULL,					// default Datenbank
								SC_MANAGER_ALL_ACCESS );// Zugriff auf alles

	if( hSCManager == NULL )
	{
		// Fehler bei der Funktion OpenSCManager aufgetreten
		// Nun schau mer mal welcher Fehler es den war?
		dwRetCode = GetLastError();
	}
	else
	{
		// Bis jetzt ist kein Fehler aufgetreten
		// Nun den Treiber Oeffnen
		hService = OpenService( hSCManager,
								szDriverName,
								SERVICE_ALL_ACCESS );

		if( hService == NULL )
		{
			dwRetCode = GetLastError();
		}
		else
		{
			// Bis hier war noch kein Fehler
			// Hier wird der Service gestartet
			bRet = StartService( hService, 0, NULL );

			if( bRet == FALSE )
			{
				dwRetCode = GetLastError();
			}

			// Handle wieder zurueckgeben
			CloseServiceHandle( hService );

		} // Ende hService

		// Handle wieder zurueckgeben
		CloseServiceHandle( hSCManager );

	} // Ende else hSCManager

    return dwRetCode;
}


//****************************************************************************
//
//  Function:		StopEADriver
//
//	Description:	See IMCEALIB.H
//
//****************************************************************************
DWORD ImcEADrvStopDriver( char* szDriverName )
{
	SC_HANDLE	hSCManager;		// Handle auf den ServiceControlManager 
	SC_HANDLE	hService;		// Handle fuer den Service
	DWORD		dwRetCode;		// Variable fuer Returncode
	BOOL		bRet;			// Variable nimmt BOOL auf

	dwRetCode = ERROR_SUCCESS;	// Am Anfang ist alles OK

	hSCManager = OpenSCManager( NULL,					// localer Rechner
								NULL,					// default Datenbank
								SC_MANAGER_ALL_ACCESS );// Zugriff auf alles

	if( hSCManager == NULL )
	{
		// Fehler bei der Funktion OpenSCManager aufgetreten
		// Nun schau mer mal welcher Fehler es den war?
		dwRetCode = GetLastError();
	}
	else
	{
		// Bis jetzt ist kein Fehler aufgetreten
		// Nun den Treiber Oeffnen
		hService = OpenService( hSCManager,
								szDriverName,
								SERVICE_ALL_ACCESS );

		if( hService == NULL )
		{
			dwRetCode = GetLastError();
		}
		else
		{
			SERVICE_STATUS ServiceStatus;

			// Bis hier war noch kein Fehler
			// Hier wird der Service gestopt
			bRet = ControlService(	hService,
									SERVICE_CONTROL_STOP,
									&ServiceStatus );

			if( bRet == FALSE )
			{
				dwRetCode = GetLastError();
			}

			// Handle wieder freigeben
			CloseServiceHandle( hService );

		} // Ende else hService

		// Handle wieder freigeben
		CloseServiceHandle( hSCManager );

	} // Ende else hSCManager

    return dwRetCode;
}


//****************************************************************************
//
//  Function: ImcEADrvOpen
//
//	Description:	See IMCEALIB.H
//
//****************************************************************************
DWORD ImcEADrvOpen( char* szDriverName, HANDLE* phDevice )
{
	DWORD		dwRetCode;
	char		completeDeviceName[256] = "";
	HANDLE		hTmpDevice;

	if(!phDevice)
		return ERROR_INVALID_PARAMETER;
	dwRetCode = ERROR_SUCCESS;		// Am Anfang ist alles OK

	// Nun den Treibernamen zusammenbauen
	strcat( completeDeviceName, "\\\\.\\" );	
	strcat( completeDeviceName, szDriverName );
		
	// Wir oeffnen den Treiber
	hTmpDevice = CreateFile(	completeDeviceName,
								GENERIC_READ | GENERIC_WRITE,
								FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
								NULL,
								OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL,
								NULL );

	if( hTmpDevice == (HANDLE)-1 )
	{
		// Fehler aufgetreten
		dwRetCode = GetLastError();
	}
	else
	{
		// Es ist alles OK
		*phDevice = hTmpDevice;

	} // Ende Handle

    return dwRetCode;
}


//****************************************************************************
//
//	Function:		ImcEADrvClose
//
//	Description:	See IMCEALIB.H
//
//****************************************************************************
DWORD ImcEADrvClose( HANDLE hDrvHandle )
{
	DWORD		dwRetCode;
	BOOL		bRet;

	if(!hDrvHandle)
		return ERROR_INVALID_HANDLE;
	dwRetCode = ERROR_SUCCESS;	// Am Anfang ist alles OK

	// Den Treiber nur schliesen wenn er auch geöffnet war
	bRet = CloseHandle( hDrvHandle );
	if( bRet != TRUE )
	{
		// Fehler beim schliesen aufgetreten
		dwRetCode = GetLastError();
	}

    return dwRetCode;
}


//****************************************************************************
//
//	Function:		ImcEADrvGetVersion
//
//	Description:	See IMCEALIB.H
//
//****************************************************************************
DWORD ImcEADrvGetVersion( HANDLE hDrvHandle, PGETVERSION pGetVersion )
{
	DWORD		dwRetCode;			// Returncode
	BOOL		bRet;				// Returncode 
	ULONG		cbBytesReturned;	// Anzahl der Rueckgabebytes

	if(!hDrvHandle)
		return ERROR_INVALID_HANDLE;
	dwRetCode = ERROR_SUCCESS;		// Am Anfang ist alles OK

	// Nun kann mit den Handle gearbeitet werden
	bRet = DeviceIoControl( hDrvHandle,					// Handle vom Treiber
							(DWORD)CTRLCD_GET_DRIVER_VERSION,	// Funktion
							NULL,						// Vorgabepuffer
							0,							// Laenge
							(LPVOID)pGetVersion,		// Rueckgabepuffer
							sizeof(GETVERSION),			// Laenge
							&cbBytesReturned,			// Rueckgabebytes
							NULL );

	if( bRet == FALSE )
	{
		// Fehler beim Aufruf
		dwRetCode = GetLastError();
	} 
	else
	{
		if( cbBytesReturned != sizeof(GETVERSION) )
		{
			// Anzahl der uebergebenene Bytes ist nicht korrekt
			dwRetCode = ERROR_OUTOFMEMORY;
		}
	} // Ende bRet

    return dwRetCode;
}


//****************************************************************************
//
//	Function:		ImcEADrvReservePort
//
//	Description:	See IMCEALIB.H
//
//****************************************************************************
DWORD ImcEADrvReservePort( HANDLE hDrvHandle, PRESERVEIO pReserveIO )
{
	DWORD		dwRetCode;			// Returncode
	BOOL		bRet;				// Returncode 
	ULONG		cbBytesReturned;	// Anzahl der Rueckgabebytes

	if(!hDrvHandle)
		return ERROR_INVALID_HANDLE;
	dwRetCode = ERROR_SUCCESS;		// Am Anfang ist alles OK

	// Nun kann mit den Handle gearbeitet werden
	bRet = DeviceIoControl( hDrvHandle,				// Handle vom Treiber
							(DWORD)CTRLCD_RESERVE_PORT,	// Funktion
							(LPVOID)pReserveIO,		// Vorgabepuffer
							sizeof(RESERVEIO),		// Laenge
							(LPVOID)pReserveIO,		// Rueckgabepuffer
							sizeof(RESERVEIO),		// Laenge
							&cbBytesReturned,		// Rueckgabebytes
							NULL );

	if( bRet == FALSE )
	{
		// Fehler beim Aufruf
		dwRetCode = GetLastError();
	}
	else
	{
		if( cbBytesReturned != sizeof(RESERVEIO) )
		{
			// Die Anzahl der zurueckgelieferten Bytes 
			// ist nicht korrekt.
			dwRetCode = ERROR_OUTOFMEMORY;
		}
	} // Ende bRet

    return dwRetCode;
}


//****************************************************************************
//
//	Function:		ImcEADrvFreePort
//
//	Description:	See IMCEALIB.H
//
//****************************************************************************
DWORD ImcEADrvFreePort( HANDLE hDrvHandle, PRESERVEIO pReserveIO )
{
	DWORD		dwRetCode;			// Returncode
	BOOL		bRet;				// Returncode 
	ULONG		cbBytesReturned;	// Anzahl der Rueckgabebytes

	if(!hDrvHandle)
		return ERROR_INVALID_HANDLE;
	dwRetCode = ERROR_SUCCESS;		// Am Anfang ist alles OK

	// Nun kann mit den Handle gearbeitet werden
	bRet = DeviceIoControl( hDrvHandle,			// Handle vom Treiber
							(DWORD)CTRLCD_FREE_PORT,		// Funktion
							(LPVOID)pReserveIO,		// Vorgabepuffer
							sizeof(RESERVEIO),		// Laenge
							(LPVOID)NULL,			// Rueckgabepuffer
							(DWORD)0,				// Laenge
							&cbBytesReturned,		// Rueckgabebytes
							NULL );

	if( bRet == FALSE )
	{
		// Fehler beim Aufruf
		dwRetCode = GetLastError();
	}
	else
	{
		if( cbBytesReturned != 0 )
		{
			// Die Anzahl der zurueckgelieferten Bytes 
			// ist nicht korrekt.
			dwRetCode = ERROR_OUTOFMEMORY;
		}
	} // Ende bRet

    return dwRetCode;
}


//****************************************************************************
//
//	Function:		ImcEADrvWritePort
//
//	Description:	See IMCEALIB.H
//
//****************************************************************************
DWORD ImcEADrvWritePort( HANDLE hDrvHandle, PPORTIO pPortIO )
{
	DWORD		dwRetCode;			// Returncode
	BOOL		bRet;				// Returncode 
	ULONG		cbBytesReturned;	// Anzahl der Rueckgabebytes

	if(!hDrvHandle)
		return ERROR_INVALID_HANDLE;
	dwRetCode = ERROR_SUCCESS;		// Am Anfang ist alles OK

	// Nun kann mit den Handle gearbeitet werden
	bRet = DeviceIoControl( hDrvHandle,				// Handle vom Treiber
							(DWORD)CTRLCD_WRITE_PORT,		// Funktion
							(LPVOID)pPortIO,		// Vorgabepuffer
							sizeof(PORTIO),			// Laenge
							(LPVOID)pPortIO,		// Rueckgabepuffer
							sizeof(PORTIO),			// Laenge
							&cbBytesReturned,		// Rueckgabebytes
							NULL );

	if( bRet == FALSE )
	{
		// Fehler beim Aufruf
		dwRetCode = GetLastError();
	}
	else
	{
		if( cbBytesReturned != sizeof(PORTIO) )
		{
			// Die Anzahl der zurueckgelieferten Bytes 
			// ist nicht korrekt.
			dwRetCode = ERROR_OUTOFMEMORY;
		}
	} // Ende bRet

    return dwRetCode;
}

//****************************************************************************
//
//	Function:		ImcEADrvReadPort
//
//	Description:	See IMCEALIB.H
//
//****************************************************************************
DWORD ImcEADrvReadPort( HANDLE hDrvHandle, PPORTIO pPortIO )
{
	DWORD		dwRetCode;			// Returncode
	BOOL		bRet;				// Returncode 
	ULONG		cbBytesReturned;	// Anzahl der Rueckgabebytes

	if(!hDrvHandle)
		return ERROR_INVALID_HANDLE;
	dwRetCode = ERROR_SUCCESS;		// Am Anfang ist alles OK

	// Nun kann mit den Handle gearbeitet werden
	bRet = DeviceIoControl( hDrvHandle,				// Handle vom Treiber
							(DWORD)CTRLCD_READ_PORT,		// Funktion
							(LPVOID)pPortIO,		// Vorgabepuffer
							sizeof(PORTIO),			// Laenge
							(LPVOID)pPortIO,		// Rueckgabepuffer
							sizeof(PORTIO),			// Laenge
							&cbBytesReturned,		// Rueckgabebytes
							NULL );

	if( bRet == FALSE )
	{
		// Fehler beim Aufruf
		dwRetCode = GetLastError();
	}
	else
	{
		if( cbBytesReturned != sizeof(PORTIO) )
		{
			// Die Anzahl der zurueckgelieferten Bytes 
			// ist nicht korrekt.
			dwRetCode = ERROR_OUTOFMEMORY;
		}

	} // Ende bRet

    return dwRetCode;
}


//****************************************************************************
//
//	Function:		ImcEADrvReserveMem
//
//	Description:	See IMCEALIB.H
//
//****************************************************************************
DWORD ImcEADrvReserveMem( HANDLE hDrvHandle, PRESERVEIO pReserveIO )
{
	DWORD		dwRetCode;			// Returncode
	BOOL		bRet;				// Returncode 
	ULONG		cbBytesReturned;	// Anzahl der Rueckgabebytes

	if(!hDrvHandle)
		return ERROR_INVALID_HANDLE;
	dwRetCode = ERROR_SUCCESS;		// Am Anfang ist alles OK

	// Nun kann mit den Handle gearbeitet werden
	bRet = DeviceIoControl( hDrvHandle,				// Handle vom Treiber
							(DWORD)CTRLCD_RESERVE_MEM,		// Funktion
							(LPVOID)pReserveIO,		// Vorgabepuffer
							sizeof(RESERVEIO),		// Laenge
							(LPVOID)pReserveIO,		// Vorgabepuffer
							sizeof(RESERVEIO),		// Laenge
							&cbBytesReturned,		// Rueckgabebytes
							NULL );

	if( bRet == FALSE )
	{
		// Fehler beim Aufruf
		dwRetCode = GetLastError();
	}
	else
	{
		if( cbBytesReturned != sizeof(RESERVEIO) )
		{
			// Die Anzahl der zurueckgelieferten Bytes 
			// ist nicht korrekt.
			dwRetCode = ERROR_OUTOFMEMORY;
		}
	} // Ende bRet


    return dwRetCode;
}

DWORD ImcEADrvReserveMemEx( HANDLE hDrvHandle, PRESERVEIO pReserveIO )
{
	DWORD		dwRetCode;			// Returncode
	BOOL		bRet;				// Returncode 
	ULONG		cbBytesReturned;	// Anzahl der Rueckgabebytes

	if(!hDrvHandle)
		return ERROR_INVALID_HANDLE;
	dwRetCode = ERROR_SUCCESS;		// Am Anfang ist alles OK

	// Nun kann mit den Handle gearbeitet werden
	bRet = DeviceIoControl( hDrvHandle,				// Handle vom Treiber
							(DWORD)CTRLCD_RESERVE_MEM_EX,	// Funktion
							(LPVOID)pReserveIO,		// Vorgabepuffer
							sizeof(RESERVEIO),		// Laenge
							(LPVOID)pReserveIO,		// Vorgabepuffer
							sizeof(RESERVEIO),		// Laenge
							&cbBytesReturned,		// Rueckgabebytes
							NULL );

	if( bRet == FALSE )
	{
		// Fehler beim Aufruf
		dwRetCode = GetLastError();
	}
	else
	{
		if( cbBytesReturned != sizeof(RESERVEIO) )
		{
			// Die Anzahl der zurueckgelieferten Bytes 
			// ist nicht korrekt.
			dwRetCode = ERROR_OUTOFMEMORY;
		}
	} // Ende bRet


    return dwRetCode;
}


//****************************************************************************
//
//	Function:		ImcEADrvFreeMem
//
//	Description:	See IMCEALIB.H
//
//****************************************************************************
DWORD ImcEADrvFreeMem( HANDLE hDrvHandle, PRESERVEIO pReserveIO )
{
	DWORD		dwRetCode;			// Returncode
	BOOL		bRet;				// Returncode 
	ULONG		cbBytesReturned;	// Anzahl der Rueckgabebytes

	if(!hDrvHandle)
		return ERROR_INVALID_HANDLE;
	dwRetCode = ERROR_SUCCESS;		// Am Anfang ist alles OK

	// Nun kann mit den Handle gearbeitet werden
	bRet = DeviceIoControl( hDrvHandle,				// Handle vom Treiber
							(DWORD)CTRLCD_FREE_MEM,		// Funktion
							(LPVOID)pReserveIO,		// Vorgabepuffer
							sizeof(RESERVEIO),		// Laenge
							NULL,					// Vorgabepuffer
							0,						// Laenge
							&cbBytesReturned,		// Rueckgabebytes
							NULL );

	if( bRet == FALSE )
	{
		// Fehler beim Aufruf
		dwRetCode = GetLastError();

	}
	else
	{
		if( cbBytesReturned != 0 )
		{
			// Die Anzahl der zurueckgelieferten Bytes 
			// ist nicht korrekt.
			dwRetCode = ERROR_OUTOFMEMORY;
		}
	} // Ende bRet

    return dwRetCode;
}


//****************************************************************************
//
//	Function:		ImcEADrvStartInterruptHandler
//
//	Description:	See IMCEALIB.H
//
//****************************************************************************
DWORD ImcEADrvStartInterruptHandler( HANDLE hDrvHandle, PRESERVEINT pInt )
{
	DWORD		dwRetCode;			// Returncode
	BOOL		bRet;				// Returncode 
	ULONG		cbBytesReturned;	// Anzahl der Rueckgabebytes

	if(!hDrvHandle)
		return ERROR_INVALID_HANDLE;
	dwRetCode = ERROR_SUCCESS;		// Am Anfang ist alles OK

	// Nun kann mit den Handle gearbeitet werden
	bRet = DeviceIoControl( hDrvHandle,					// Handle vom Treiber
							(DWORD)CTRLCD_START_INTR_HANDLER,	// Funktion
							(LPVOID)pInt,				// Vorgabepuffer
							sizeof(RESERVEINT),			// Laenge
							(LPVOID)pInt,				// Rueckgabepuffer
							sizeof(RESERVEINT),			// Laenge
							&cbBytesReturned,			// Rueckgabebytes
							NULL );

	if( bRet == FALSE )
	{
		// Fehler beim Aufruf
		dwRetCode = GetLastError();
	}
	else
	{
		if( cbBytesReturned != sizeof(RESERVEINT) )
		{
			// Die Anzahl der zurueckgelieferten Bytes 
			// ist nicht korrekt.
			dwRetCode = ERROR_OUTOFMEMORY;
		}
	} // Ende bRet

    return dwRetCode;
}

//****************************************************************************
//
//	Function:		ImcEADrvStopInterruptHandler
//
//	Description:	See IMCEALIB.H
//
//****************************************************************************
DWORD ImcEADrvStopInterruptHandler( HANDLE hDrvHandle, PRESERVEINT pIntIn )
{
	DWORD		dwRetCode;			// Returncode
	BOOL		bRet;				// Returncode 
	ULONG		cbBytesReturned;	// Anzahl der Rueckgabebytes

	if(!hDrvHandle)
		return ERROR_INVALID_HANDLE;
	dwRetCode = ERROR_SUCCESS;		// Am Anfang ist alles OK

	// Nun kann mit den Handle gearbeitet werden
	bRet = DeviceIoControl( hDrvHandle,					// Handle vom Treiber
							(DWORD)CTRLCD_FREE_INTR_HANDLER,	// Funktion
							(LPVOID)pIntIn,				// Vorgabepuffer
							sizeof(RESERVEINT),			// Laenge
							NULL,						// Vorgabepuffer
							0,							// Laenge
							&cbBytesReturned,			// Rueckgabebytes
							NULL );

	if( bRet == FALSE )
	{
		// Fehler beim Aufruf
		dwRetCode = GetLastError();
	}
	else
	{
		if( cbBytesReturned != 0 )
		{
			// Die Anzahl der zurueckgelieferten Bytes 
			// ist nicht korrekt.
			dwRetCode = ERROR_OUTOFMEMORY;
		}
	} // Ende bRet

    return dwRetCode;
}

//****************************************************************************
//
//	Function:		ImcEADrvWaitForInterrupt
//
//	Description:	See IMCEALIB.H
//
//****************************************************************************
DWORD ImcEADrvWaitForInterrupt( HANDLE hDrvHandle, PRESERVEINT pInt )
{
	DWORD		dwRetCode;			// Returncode
	BOOL		bRet;				// Returncode 
	ULONG		cbBytesReturned;	// Anzahl der Rueckgabebytes

	if(!hDrvHandle)
		return ERROR_INVALID_HANDLE;
	dwRetCode = ERROR_SUCCESS;		// Am Anfang ist alles OK

	bRet = DeviceIoControl( hDrvHandle,					// Handle vom Treiber
							(DWORD)CTRLCD_WAIT_FOR_INTERRUPT,	// Funktion
							(LPVOID)pInt,				// Vorgabepuffer
							sizeof(RESERVEINT),			// Laenge
							(LPVOID)pInt,				// Returnpuffer
							sizeof(RESERVEINT),			// Laenge
							&cbBytesReturned,			// Rueckgabebytes
							NULL ); 

	if( bRet == FALSE )
	{
		// Fehler beim Aufruf
		dwRetCode = GetLastError();
	}
	else
	{
		if( cbBytesReturned != sizeof(RESERVEINT) )
		{
			// Die Anzahl der zurueckgelieferten Bytes 
			// ist nicht korrekt.
			dwRetCode = ERROR_OUTOFMEMORY;
		}
	} // Ende bRet

    return dwRetCode;
}

//****************************************************************************
//
//	Function:		ImcEADrvWaitForInterruptEx
//
//	Description:	See IMCEALIB.H
//
//****************************************************************************
DWORD ImcEADrvWaitForInterruptEx( HANDLE hDrvHandle, PRESERVEINTEX pInt )
{
	DWORD		dwRetCode;			// Returncode
	BOOL		bRet;				// Returncode 
	ULONG		cbBytesReturned;	// Anzahl der Rueckgabebytes

	if(!hDrvHandle)
		return ERROR_INVALID_HANDLE;
	dwRetCode = ERROR_SUCCESS;		// Am Anfang ist alles OK

	bRet = DeviceIoControl( hDrvHandle,					// Handle vom Treiber
							(DWORD)CTRLCD_WAIT_FOR_INTERRUPT_EX,	// Funktion
							(LPVOID)pInt,				// Vorgabepuffer
							sizeof(RESERVEINTEX),			// Laenge
							(LPVOID)pInt,				// Returnpuffer
							sizeof(RESERVEINTEX),			// Laenge
							&cbBytesReturned,			// Rueckgabebytes
							NULL ); 

	if( bRet == FALSE )
	{
		// Fehler beim Aufruf
		dwRetCode = GetLastError();
	}
	else
	{
		if( cbBytesReturned != sizeof(RESERVEINTEX) )
		{
			// Die Anzahl der zurueckgelieferten Bytes 
			// ist nicht korrekt.
			dwRetCode = ERROR_OUTOFMEMORY;
		}
	} // Ende bRet

    return dwRetCode;
}

//****************************************************************************
//
//	Function:		ImcEADrvResetWaitForInterrupt
//
//	Description:	See IMCEALIB.H
//
//****************************************************************************
DWORD ImcEADrvResetWaitForInterrupt( HANDLE hDrvHandle, PRESERVEINT pInt )
{
	DWORD		dwRetCode;			// Returncode
	BOOL		bRet;				// Returncode 
	ULONG		cbBytesReturned;	// Anzahl der Rueckgabebytes

	if(!hDrvHandle)
		return ERROR_INVALID_HANDLE;
	dwRetCode = ERROR_SUCCESS;		// Am Anfang ist alles OK

	// Nun kann mit den Handle gearbeitet werden
	bRet = DeviceIoControl( hDrvHandle,					// Handle vom Treiber
							(DWORD)CTRLCD_RESET_WAIT_FOR_INTERRUPT,	// Funktion
							(LPVOID)pInt,				// Vorgabepuffer
							sizeof(RESERVEINT),			// Laenge
							NULL,						// Vorgabepuffer
							0,							// Laenge
							&cbBytesReturned,			// Rueckgabebytes
							NULL );

	if( bRet == FALSE )
	{
		// Fehler beim Aufruf
		dwRetCode = GetLastError();
	}
	else
	{
		if( cbBytesReturned != 0 )
		{
			// Die Anzahl der zurueckgelieferten Bytes 
			// ist nicht korrekt.
			dwRetCode = ERROR_OUTOFMEMORY;
		}
	} // Ende bRet

    return dwRetCode;
}
//****************************************************************************
//
//	Function:		ImcEADrvResetWaitForInterrupt
//
//	Description:	See IMCEALIB.H
//
//****************************************************************************
DWORD ImcEADrvSearchPCI( HANDLE hDrvHandle,
                         ULONG	VendorID,
                         ULONG	DeviceID,
                         ULONG	SubVendorID,
                         ULONG	SubDeviceID,
                         PCI_CONFIG_SPACE *ConfigSpace,
                         ULONG	MaxNumberOfFunctions,
                         ULONG *NumOfFunctions)
{
	DWORD		dwRetCode;			// Returncode
	BOOL		bRet;				// Returncode 
	ULONG		cbBytesReturned;	// Anzahl der Rueckgabebytes
    SEARCHPCI   Param;

	if(!hDrvHandle)
		return ERROR_INVALID_HANDLE;
	dwRetCode = ERROR_SUCCESS;		// Am Anfang ist alles OK

	// Nun kann mit den Handle gearbeitet werden
    Param.VendorID = VendorID;
	Param.DeviceID = DeviceID;
	Param.SubVendorID = SubVendorID;
	Param.SubDeviceID = SubDeviceID;
	Param.MaxNumberOfFunctions = MaxNumberOfFunctions;

	bRet = DeviceIoControl( hDrvHandle,					// Handle vom Treiber
							(DWORD)CTRLCD_SEARCH_PCI,	// Funktion
							(LPVOID)&Param,				// Vorgabepuffer
							sizeof(Param),			// Laenge
							ConfigSpace,						// Vorgabepuffer
							MaxNumberOfFunctions*sizeof(PCI_CONFIG_SPACE),							// Laenge
							&cbBytesReturned,			// Rueckgabebytes
							NULL );

	if( bRet == FALSE )
	{
		// Fehler beim Aufruf
		dwRetCode = GetLastError();
	}
	else
	{
		*NumOfFunctions = cbBytesReturned/sizeof(PCI_CONFIG_SPACE);
	} // Ende bRet

    return dwRetCode;
}
//****************************************************************************
//
//	Function:		ImcEADrvGetPhysAdr
//
//	Description:	See IMCEALIB.H
//
//****************************************************************************
DWORD ImcEADrvGetPhysAdr( HANDLE hDrvHandle, PRESERVEPHY pReservePhy )
{
	DWORD		dwRetCode;			// Returncode
	BOOL		bRet;				// Returncode 
	ULONG		cbBytesReturned;	// Anzahl der Rueckgabebytes

	if(!hDrvHandle)
		return ERROR_INVALID_HANDLE;
	dwRetCode = ERROR_SUCCESS;		// Am Anfang ist alles OK

	// Nun kann mit den Handle gearbeitet werden
	bRet = DeviceIoControl( hDrvHandle,				// Handle vom Treiber
							(DWORD)CTRLCD_GET_PHYS_ADR,	// Funktion
							(LPVOID)pReservePhy,		// Vorgabepuffer
							sizeof(RESERVEPHY),		// Laenge
							(LPVOID)pReservePhy,		// Vorgabepuffer
							sizeof(RESERVEPHY),		// Laenge
							&cbBytesReturned,		// Rueckgabebytes
							NULL );

	if( bRet == FALSE )
	{
		// Fehler beim Aufruf
		dwRetCode = GetLastError();
	}
	else
	{
		if( cbBytesReturned != sizeof(RESERVEPHY) )
		{
			// Die Anzahl der zurueckgelieferten Bytes 
			// ist nicht korrekt.
			dwRetCode = ERROR_OUTOFMEMORY;
		}
	} // Ende bRet



    return dwRetCode;
}

//****************************************************************************
//
//	Function:		ImcEADrvGetPhysAdr
//
//	Description:	See IMCEALIB.H
//
//****************************************************************************
DWORD ImcEADrvFreePhysAdr( HANDLE hDrvHandle, PRESERVEPHY pReservePhy )
{
	DWORD		dwRetCode;			// Returncode
	BOOL		bRet;				// Returncode 
	ULONG		cbBytesReturned;	// Anzahl der Rueckgabebytes

	if(!hDrvHandle)
		return ERROR_INVALID_HANDLE;
	dwRetCode = ERROR_SUCCESS;		// Am Anfang ist alles OK

	// Nun kann mit den Handle gearbeitet werden
	bRet = DeviceIoControl( hDrvHandle,				// Handle vom Treiber
							(DWORD)CTRLCD_FREE_PHYS_ADR,	// Funktion
							(LPVOID)pReservePhy,		// Vorgabepuffer
							sizeof(RESERVEPHY),		// Laenge
							(LPVOID)NULL,		// Vorgabepuffer
							0,		// Laenge
							&cbBytesReturned,		// Rueckgabebytes
							NULL );

	if( bRet == FALSE )
	{
		// Fehler beim Aufruf
		dwRetCode = GetLastError();
	}
	else
	{
		if( cbBytesReturned != sizeof(RESERVEPHY) )
		{
			// Die Anzahl der zurueckgelieferten Bytes 
			// ist nicht korrekt.
			dwRetCode = ERROR_OUTOFMEMORY;
		}
	} // Ende bRet



    return dwRetCode;
}

DWORD ImcEADrvDoIntr( HANDLE hDrvHandle, PDOINTERRUPT pDoIntr )
{
	DWORD		dwRetCode;			// Returncode
	BOOL		bRet;				// Returncode 
	ULONG		cbBytesReturned;	// Anzahl der Rueckgabebytes

	if(!hDrvHandle)
		return ERROR_INVALID_HANDLE;
	dwRetCode = ERROR_SUCCESS;		// Am Anfang ist alles OK

	// Nun kann mit den Handle gearbeitet werden
	bRet = DeviceIoControl( hDrvHandle,				// Handle vom Treiber
							(DWORD)CTRLCD_DO_INTR,		// Funktion
							(LPVOID)pDoIntr,		// Vorgabepuffer
							sizeof(DOINTERRUPT),		// Laenge
							NULL,					// Vorgabepuffer
							0,						// Laenge
							&cbBytesReturned,		// Rueckgabebytes
							NULL );

	if( bRet == FALSE )
	{
		// Fehler beim Aufruf
		dwRetCode = GetLastError();

	}
	else
	{
		if( cbBytesReturned != 0 )
		{
			// Die Anzahl der zurueckgelieferten Bytes 
			// ist nicht korrekt.
			dwRetCode = ERROR_OUTOFMEMORY;
		}
	} // Ende bRet

    return dwRetCode;
}
