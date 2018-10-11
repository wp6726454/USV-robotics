/////////////////////////////////////////////////////////////////////////////
//                      Copyright (c) 1995  Siemens AG
//
// Modulname:		IMCDRV.H
//
// Beschreibung:	Gemeinsame Definitionen für die Treiber 
//					der IMC-Baugruppen
//
// Revision History:
// -----------------
//  16.04.1997:  Erstellt,              T. Troester, AUT GT11
//  16.09.1997:  DriverName, DriverFile -> define.
//  08.06.2011:  DriverName for 32- and 64-bit is different
/////////////////////////////////////////////////////////////////////////////

#ifndef _imcdrv_h
#define _imcdrv_h

#ifndef ULONG_PTR
#if defined(_WIN64)
    typedef unsigned __int64 ULONG_PTR, *PULONG_PTR;
#else
    typedef unsigned long ULONG_PTR, *PULONG_PTR;
#endif
#endif

// ============================================================================
// Definitionen fuer den EA-Treiber
// ============================================================================

// Registriername und Pfad des Treibers 
#define DRIVER_PATH				"\\system32\\drivers\\"
#define DRIVER_NAME_EA			"IMCEA"
#define DRIVER_NAME_EA0			"IMCEA0"
#define DRIVER_NAME_EA1			"IMCEA1"
#define DRIVER_NAME_EA2			"IMCEA2"
#define DRIVER_NAME_EA3			"IMCEA3"
#define DRIVER_NAME_EA4			"IMCEA4"
#define DRIVER_FILE_EA			"IMCEA.SYS"
#define DRIVER_FILE_EA_32		"I386\\IMCEA.SYS"
#define DRIVER_FILE_EA_64		"AMD64\\IMCEA.SYS"
#define DRIVER_EA_LOG_REG_PATH	"SYSTEM\\CURRENTCONTROLSET\\SERVICES\\EVENTLOG\\SYSTEM\\IMCEA"

#define	PORT_MODE_UCHAR		0
#define	PORT_MODE_USHORT	1
#define	PORT_MODE_ULONG		2

#define INT_MODE_EXCLUSIVE	0
#define	INT_MODE_SHARED		1

#define INT_BUS_ISA     	0
#define	INT_BUS_PCI 		0x100

#define	INT_LEVELSENSITIVE	0
#define INT_LATCHED     	0x200

#define INT_NORMAL			0
#define INT_BREAK			1
#define INT_TIMEOUT			2

#define	INT_MODE_IO_READ8		1
#define	INT_MODE_IO_READ16		2
#define	INT_MODE_IO_READ32		3
#define	INT_MODE_MEM_READ8		4
#define	INT_MODE_MEM_READ16		5
#define	INT_MODE_MEM_READ32		6

// Definition von Gerätetyp-Wert (32768...65535).
// (Werte im Bereich 0...32767 sind "Microsoft Corporation" vorbehalten)
#define FILE_DEVICE_EA		0x00008500

// IOCTL Funktions-Codes (0x0800..0x0fff => 2048..4095)
// (Werte im Bereich 0..2047 sind "Microsoft Corporation" vorbehalten)
#define DEVCTRL_GET_DRIVER_VERSION			0x0810
#define DEVCTRL_GET_DRIVER_INFO				0x0811

#define DEVCTRL_RESERVE_PORT				0x0812	
#define DEVCTRL_READ_PORT					0x0813
#define DEVCTRL_WRITE_PORT					0x0814
#define DEVCTRL_FREE_PORT					0x0815	

#define DEVCTRL_RESERVE_MEM					0x0816	
#define DEVCTRL_FREE_MEM					0x0817

#define DEVCTRL_START_INTR_HANDLER			0x0818
#define DEVCTRL_FREE_INTR_HANDLER			0x0819
#define DEVCTRL_WAIT_FOR_INTERRUPT			0x0820
#define DEVCTRL_RESET_WAIT_FOR_INTERRUPT	0x0821

#define DEVCTRL_SEARCH_PCI  				0x0824
#define DEVCTRL_WAIT_FOR_INTERRUPT_EX		0x0825

#define DEVCTRL_GET_PHYS_ADR 		    	0x0830
#define DEVCTRL_FREE_PHYS_ADR 		    	0x0831
#define DEVCTRL_DO_INTR      		    	0x0832
#define DEVCTRL_RESERVE_MEM_EX				0x0833	

// I/O-ControlCodes fuer Funktionen im Kerneltreiber fur die IMC-Baugruppe
//
//    Inhalt |      DeviceType      | Access |  IOCTL-Function  | Method |
//    Bits    31..................15 14....13 12...............2 1......0
     
#define CTRLCD_GET_DRIVER_VERSION		CTL_CODE( FILE_DEVICE_EA,			\
											DEVCTRL_GET_DRIVER_VERSION,		\
											METHOD_BUFFERED,				\
											FILE_ANY_ACCESS  )

#define CTRLCD_GET_DRIVER_INFO			CTL_CODE( FILE_DEVICE_EA,			\
                                              DEVCTRL_GET_DRIVER_INFO,		\
                                              METHOD_BUFFERED,				\
                                              FILE_ANY_ACCESS  )

#define CTRLCD_RESERVE_PORT				CTL_CODE( FILE_DEVICE_EA,			\
                                              DEVCTRL_RESERVE_PORT,			\
                                              METHOD_BUFFERED,				\
                                              FILE_ANY_ACCESS  )

#define CTRLCD_READ_PORT				CTL_CODE( FILE_DEVICE_EA,			\
                                              DEVCTRL_READ_PORT,			\
                                              METHOD_BUFFERED,				\
                                              FILE_ANY_ACCESS  )

#define CTRLCD_WRITE_PORT				CTL_CODE( FILE_DEVICE_EA,			\
                                              DEVCTRL_WRITE_PORT,			\
                                              METHOD_BUFFERED,				\
                                              FILE_ANY_ACCESS  )

#define CTRLCD_FREE_PORT				CTL_CODE( FILE_DEVICE_EA,			\
                                              DEVCTRL_FREE_PORT,			\
                                              METHOD_BUFFERED,				\
                                              FILE_ANY_ACCESS  )

#define CTRLCD_RESERVE_MEM				CTL_CODE( FILE_DEVICE_EA,			\
                                              DEVCTRL_RESERVE_MEM,			\
                                              METHOD_BUFFERED,				\
                                              FILE_ANY_ACCESS  )

#define CTRLCD_RESERVE_MEM_EX			CTL_CODE( FILE_DEVICE_EA,			\
                                              DEVCTRL_RESERVE_MEM_EX,		\
                                              METHOD_BUFFERED,				\
                                              FILE_ANY_ACCESS  )

#define CTRLCD_FREE_MEM					CTL_CODE( FILE_DEVICE_EA,			\
                                              DEVCTRL_FREE_MEM,				\
                                              METHOD_BUFFERED,				\
                                              FILE_ANY_ACCESS  )

#define CTRLCD_START_INTR_HANDLER   CTL_CODE( FILE_DEVICE_EA,				\
                                              DEVCTRL_START_INTR_HANDLER,	\
                                              METHOD_BUFFERED,				\
                                              FILE_ANY_ACCESS  )

#define CTRLCD_FREE_INTR_HANDLER   CTL_CODE( FILE_DEVICE_EA,				\
                                              DEVCTRL_FREE_INTR_HANDLER,	\
                                              METHOD_BUFFERED,				\
                                              FILE_ANY_ACCESS  )

#define CTRLCD_WAIT_FOR_INTERRUPT		CTL_CODE( FILE_DEVICE_EA,			\
                                              DEVCTRL_WAIT_FOR_INTERRUPT,	\
                                              METHOD_BUFFERED,				\
                                              FILE_ANY_ACCESS  )

#define CTRLCD_RESET_WAIT_FOR_INTERRUPT	CTL_CODE( FILE_DEVICE_EA,			\
                                              DEVCTRL_RESET_WAIT_FOR_INTERRUPT,  \
                                              METHOD_BUFFERED,				\
                                              FILE_ANY_ACCESS  )

#define CTRLCD_SEARCH_PCI           	CTL_CODE( FILE_DEVICE_EA,			\
                                              DEVCTRL_SEARCH_PCI,           \
                                              METHOD_BUFFERED,				\
                                              FILE_ANY_ACCESS  )

#define CTRLCD_WAIT_FOR_INTERRUPT_EX	CTL_CODE( FILE_DEVICE_EA,			\
                                              DEVCTRL_WAIT_FOR_INTERRUPT_EX,	\
                                              METHOD_BUFFERED,				\
                                              FILE_ANY_ACCESS  )
#define CTRLCD_GET_PHYS_ADR	  	       CTL_CODE( FILE_DEVICE_EA,			\
                                              DEVCTRL_GET_PHYS_ADR,			\
                                              METHOD_BUFFERED,				\
                                              FILE_ANY_ACCESS  )
#define CTRLCD_FREE_PHYS_ADR	  	       CTL_CODE( FILE_DEVICE_EA,			\
                                              DEVCTRL_FREE_PHYS_ADR,			\
                                              METHOD_BUFFERED,				\
                                              FILE_ANY_ACCESS  )

#define CTRLCD_DO_INTR	  	       CTL_CODE( FILE_DEVICE_EA,			\
                                              DEVCTRL_DO_INTR,			\
                                              METHOD_BUFFERED,				\
                                              FILE_ANY_ACCESS  )


#ifdef _MSC_VER
#pragma pack(1)         /* set all to  8-bit alignment */
#endif
#ifdef __BORLANDC__
#pragma option -a1      /* set all to  8-bit alignment */
#endif /* __BORLANDC__ */

// Strukturen und Typdefinitionen fuer die Uebergabeparameter

// --------------------------------------------------------------------------
// Struktur fuer die Reservierung von Ports und Speicher
// --------------------------------------------------------------------------
typedef struct _ReserveIO
{   
    PVOID  StartAddress;
    ULONG  Length;
#if 0
    ULONG  BusNumber;
    ULONG  BusType;
#endif
} RESERVEIO;

typedef RESERVEIO *PRESERVEIO;


//*** physikalischen non cached memory anfordern ***
typedef struct _ReservePHY
{   
    ULONG  Length;    //IN  Laenge
    ULONG  PhysAddr;  //OUT phys. Adresse
    PVOID  VirtAddr;  //OUT Virtuelle Adresse
} RESERVEPHY;

typedef RESERVEPHY *PRESERVEPHY;

// --------------------------------------------------------------------------
// Struktur fuer den Zugriff auf Ports
// --------------------------------------------------------------------------
typedef struct _PortIO
{   
	PVOID	Address;
    ULONG	Value;
	ULONG	Mode;
} PORTIO;

typedef PORTIO *PPORTIO;

// --------------------------------------------------------------------------
// Struktur fuer die Interruptbehandlung
// --------------------------------------------------------------------------
typedef struct _ReserveInt
{   
    ULONG	IntNr;
	ULONG	Mode;
#if 0
	ULONG	BusNumber;
	ULONG	BusType;
#endif
	ULONG	Timeout;
	ULONG_PTR	IntHandle;
	ULONG	Status;
} RESERVEINT;

typedef RESERVEINT *PRESERVEINT;

// --------------------------------------------------------------------------
// Struktur fuer die ext. Interruptbehandlung
// --------------------------------------------------------------------------
typedef struct _ReserveIntEx
{   
    ULONG_PTR	Address;
	ULONG	Mode;
	ULONG	Timeout;
	ULONG_PTR	IntHandle;
	ULONG	Status;
	ULONG	Value;
} RESERVEINTEX;

typedef RESERVEINTEX *PRESERVEINTEX;

// --------------------------------------------------------------------------
// Struktur fuer die PCI Searchfunktion
// --------------------------------------------------------------------------
typedef struct {
  unsigned char  PciBusNum;
  unsigned char  PciDeviceNum;
  unsigned char  PciFunctionNum;
  unsigned short VendorId;
  unsigned short DeviceId;
  unsigned short CmdReg;
  unsigned short StatReg;
  unsigned char  RevisionId;
  unsigned char  ClassCode[3];
  unsigned char  CacheLineSize;
  unsigned char  Latencytime;
  unsigned char  HeaderType;
  unsigned char  BIST;
  unsigned int   BaseAdr0;
  unsigned int   BaseAdr1;
  unsigned int   BaseAdr2;
  unsigned int   BaseAdr3;
  unsigned int   BaseAdr4;
  unsigned int   BaseAdr5;
  unsigned int   CardbusCisPtr;
  unsigned short SubsysVendorId;
  unsigned short SubsysId;
  unsigned int   ExpRomBaseAdr;
  unsigned int   reserved[2];
  unsigned char  IntLine;
  unsigned char  IntPin;
  unsigned char  Min_Gnt;
  unsigned char  Max_Lat;
} PCI_CONFIG_SPACE;
  
typedef struct _SearchPCI
{   
	ULONG	VendorID;
	ULONG	DeviceID;
	ULONG	SubVendorID;
	ULONG	SubDeviceID;
	ULONG	MaxNumberOfFunctions;
} SEARCHPCI;

typedef SEARCHPCI *PSEARCHPCI;


// --------------------------------------------------------------------------
// Struktur fuer die Version
// --------------------------------------------------------------------------
typedef struct _GetVersion
{   
	ULONG	Major;
    ULONG	Minor;
	ULONG	Intern;
} GETVERSION;

typedef GETVERSION *PGETVERSION;

typedef struct _DoInterrupt
{   
	ULONG	IntrVector;
} DOINTERRUPT;

typedef DOINTERRUPT *PDOINTERRUPT;

#ifdef _MSC_VER
#pragma pack()          /* reset alignment to commandline specification */
#endif
#ifdef __BORLANDC__
#pragma option -a.      /* reset alignment to commandline specification */
#endif /* __BORLANDC__ */

#endif  //_imcdrv_h


