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
/*  F i l e               &F: os_Env.h                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Declarations for adaption to used environment (OS, compiler, C/C++ library)
//
//****************************************************************************/
//
//  H i s t o r y :
//  ________________________________________________________________________
//
//  Date		Who				What
//  xxxx-xx-xx	Spichtinger		xx
/*****************************************************************************/

#ifndef __os_Env_h__
#define __os_Env_h__

	#ifdef __cplusplus		// if C++ compiler: Use C linkage
	extern "C"
	{
	#endif

	//########################################################################
	//  Defines
	//########################################################################

	#define	PNDEV_UM_PLATFORM_32	1
	#define	PNDEV_UM_PLATFORM_64	2
	#define	PNDEV_KM_PLATFORM_32	3
	#define	PNDEV_KM_PLATFORM_64	4
	#define	PNDEV_UM_KM_PLATFORM_32	5
	#define	PNDEV_UM_KM_PLATFORM_64	6

	//------------------------------------------------------------------------
	#if !defined (_countof)
		#define _countof(_Array)	(sizeof(_Array) / sizeof(_Array[0]))
	#endif

	#if defined (PNDEV_OS_LINUX) || defined (PNDEV_OS_WIN)
		//------------------------------------------------------------------------
		// Note:
		//	- active platform is set by user
		//		VisualStudio:	by selecting platform (ComboBox)
		//		Driver:			by selecting BuildEnvironment
		#if defined (PNDEV_DRIVER_SYS)												// compiling SYS -> KernelMode (KM)
			// #pragma message("") does not work here
			#if defined(__LP64__) || defined(_LP64) || defined (_WIN64)
				// selected platform = x64
				//#pragma message("--- platform=x64")

				#define PNDEV_PLATFORM	PNDEV_KM_PLATFORM_64
			#else
				// selected platform = Win32
				//#pragma message("--- platform=Win32")

				#define PNDEV_PLATFORM	PNDEV_KM_PLATFORM_32
			#endif
		#else																		// compiling DLL/App -> UserMode (UM)
			#define PNDEV_DRIVER_UM               //usermode when not in PNDEV_DRIVER_SYS (KM)
			#if defined(__LP64__) || defined(_LP64) || defined (_WIN64)
				// selected platform = x64
				//#pragma message("--- platform=x64")

				#define PNDEV_PLATFORM	PNDEV_UM_PLATFORM_64
			#else
				// selected platform = Win32
				//#pragma message("--- platform=Win32")

				#define PNDEV_PLATFORM	PNDEV_UM_PLATFORM_32
			#endif
		#endif
	#endif

	//------------------------------------------------------------------------
	#if defined (PNDEV_OS_WIN)
	//------------------------------------------------------------------------
	#if defined(_UNICODE) || defined(UNICODE)
		#define PNDEV_UNICODE
	#endif

	//------------------------------------------------------------------------
	#if defined (PNDEV_TOOL_GCC_MINGW)
		#define PNDEV_GCC
	#else
		typedef	UINT8	uint8_t;												// uint8_t is a synonym for UINT8 (WDF: stdint.h of C99 is not available)
		typedef	UINT16	uint16_t;
		typedef	UINT32	uint32_t;
		typedef	UINT64	uint64_t;

		typedef	INT8	int8_t;
		typedef	INT16	int16_t;
		typedef	INT32	int32_t;
		typedef	INT64	int64_t;
	#endif

	//------------------------------------------------------------------------
	#elif defined (PNDEV_OS_ADONIS)
	//------------------------------------------------------------------------
	#if (LIB_WCHAR_USAGE == YES)
		// not supported
		//#define PNDEV_UNICODE
	#endif

	#define PNDEV_PLATFORM	PNDEV_UM_KM_PLATFORM_32
	#define PNDEV_TOOL_GCC_DINKUMWARE
	#define PNDEV_GCC
	#define POSIX_OK    (0)

	#define TOLOWER
	#define	tolower		__builtin_tolower
	#define TOUPPER
	#define	toupper		__builtin_toupper

	//------------------------------------------------------------------------
	#elif defined (PNDEV_OS_LINUX)
	//------------------------------------------------------------------------
	#define PNDEV_GCC
	#define	__stdcall
	#define POSIX_OK    (0)
	#define LINUX_OK    (0)
	#define LINUX_ERROR (-1)

	typedef	uint8_t		UINT8;													// UINT8 is a synonym for uint8_t
	typedef	uint16_t	UINT16;
	typedef	uint32_t	UINT32;
	typedef	uint64_t	UINT64;

	typedef	int8_t		INT8;
	typedef	int16_t		INT16;
	typedef	int32_t		INT32;
	typedef	int64_t		INT64;

	typedef UINT16		USHORT;
	typedef UINT32		ULONG;
	typedef INT32		LONG;
	typedef	INT64		LONGLONG;
	typedef	INT64		LONG_PTR;

	typedef	void*		HANDLE;

	typedef UINT8		BOOLEAN;
	#define TRUE		1
	#define FALSE		0

	#define _ttoi		atoi

	#define Sleep(x) \
	{ \
	struct timespec uTimeSpec; \
		uTimeSpec.tv_sec    = x / 1000u;						/* sec = msec / 1000 */ \
		uTimeSpec.tv_nsec   = ((x % 1000u) * 1000u * 1000u);	/* nsec = msec*1000*1000  %1000 to eliminate the secs */ \
		nanosleep( &uTimeSpec, NULL); \
	}

	#define __forceinline __attribute__((always_inline))

	#if defined (PNDEV_DRIVER_SYS)
		// not required
	#else
		#define TOLOWER
		#define	tolower		__builtin_tolower
		#define TOUPPER
		#define	toupper		__builtin_toupper
	#endif

	//------------------------------------------------------------------------
	#elif defined (PNDEV_OS_UNKNOWN)
	//------------------------------------------------------------------------
	#endif

	//########################################################################
	#if defined (PNDEV_UNICODE)
		typedef	wchar_t	PNDEV_CHAR;
	#else
		typedef	char	PNDEV_CHAR;
	#endif

	//########################################################################
	//------------------------------------------------------------------------
	#if defined (PNDEV_TOOL_GCC_DINKUMWARE)
	//------------------------------------------------------------------------
	typedef	uint8_t		UINT8;													// UINT8 is a synonym for uint8_t
	typedef	uint16_t	UINT16;
	typedef	uint32_t	UINT32;
	typedef	uint64_t	UINT64;

	typedef	int8_t		INT8;
	typedef	int16_t		INT16;
	typedef	int32_t		INT32;
	typedef	int64_t		INT64;

	typedef	UINT8		BOOLEAN;
	typedef UINT16		USHORT;
	typedef UINT32		ULONG;
	typedef INT32		LONG;
	typedef	INT64		LONGLONG;
	typedef	INT64		LONG_PTR;

	typedef	void*		HANDLE;

	//------------------------------------------------------------------------
	#define	__stdcall
	#define __forceinline

	#define	INFINITE	(-1)													// infinite timeout

	#define Sleep(x) \
	{ \
	struct timespec uTimeSpec; \
		uTimeSpec.tv_sec    = x / 1000u;						/* sec = msec / 1000 */ \
		uTimeSpec.tv_nsec   = ((x % 1000u) * 1000u * 1000u);	/* nsec = msec*1000*1000  %1000 to eliminate the secs */ \
		nanosleep( &uTimeSpec, NULL); \
	}

	//------------------------------------------------------------------------
	#elif defined (PNDEV_TOOL_GCC_MINGW)
	//------------------------------------------------------------------------
	#define TOLOWER
	#define	tolower		__builtin_tolower
	#define TOUPPER
	#define	toupper		__builtin_toupper
	#endif

	//------------------------------------------------------------------------
	#if defined (PNDEV_GCC)
	//------------------------------------------------------------------------
	#if !(defined (PNDEV_TOOL_GCC_MINGW))
		typedef union _LARGE_INTEGER
		{
			struct
			{
				UINT32	LowPart;
				INT32	HighPart;
			}
			u;
	
			INT64 QuadPart;
		}
		LARGE_INTEGER;
	#endif

	typedef	UINT32	errno_t;
	typedef LARGE_INTEGER	PHYSICAL_ADDRESS;
	typedef UINT32			PMDL;
	//------------------------------------------------------------------------
	#define MAXUINT32	((UINT32)~((UINT32)0))

	#undef	UNREFERENCED_PARAMETER
	//#define	UNREFERENCED_PARAMETER(x)	(x)
    #define UNREFERENCED_PARAMETER(x)       (void)(x)
	#define _TRUNCATE	((size_t) -1)

	#define DMM_IFA_CODE_ATTR

	//------------------------------------------------------------------------
	#define	malloc											__builtin_malloc
	#define	free											__builtin_free
#if !(defined memset)
	#define	memset											__builtin_memset
#endif
	#define	memcpy_s(pDestIn, lSizeDestIn, pSrc, lSizeIn)	__builtin_memcpy((pDestIn), (pSrc), (lSizeIn))

	//------------------------------------------------------------------------
	#define printf		__builtin_printf
	#define scanf_s		__builtin_scanf

	#define	strcmp		__builtin_strcmp
	#define strlen		__builtin_strlen
	#define strstr		__builtin_strstr										// find a substring in a string
	#define strchr		__builtin_strchr										// find a character in a string
	#define strrchr		__builtin_strrchr										// scan a string for the last occurrence of a character

	//------------------------------------------------------------------------
	#define wcscpy_s(sDestOut, lCountDestIn, sSrcIn)				wcscpy(				(sDestOut), (sSrcIn))
	#define strcpy_s(sDestOut, lCountDestIn, sSrcIn)				__builtin_strcpy(	(sDestOut), (sSrcIn))

	#define wcscat_s(sDestInOut, lCountDestIn, sSrcIn)				wcscat(				(sDestInOut), (sSrcIn))
	#define strcat_s(sDestInOut, lCountDestIn, sSrcIn)				__builtin_strcat(	(sDestInOut), (sSrcIn))

	// Note:
	//	- strncpy()/wcsncpy():	standard implementation does not add a terminal NULL -> must be done at calling of _tcsncpy_s()
	//	- strncat_s/wcsncat_s	if lCountIn = _TRUNCATE: compute remaining size = _countof(Arg) - len(string at DestBuffer) - 1 (terminal NULL!)
	//		use lCountDestIn instead of _countof(Arg) because _countof(Arg) computes the length of a pointer if string is passed as an argument to a function (e.g. _TCHAR* sErrorOut)!
	#define wcsncpy_s(sDestOut,   lCountDestIn, sSrcIn, lCountIn)	wcsncpy(			(sDestOut), (sSrcIn), (lCountIn == _TRUNCATE ? (lCountDestIn - 1) : (lCountIn)))
	#define strncpy_s(sDestOut,   lCountDestIn, sSrcIn, lCountIn)	__builtin_strncpy(	(sDestOut), (sSrcIn), (lCountIn == _TRUNCATE ? (lCountDestIn - 1) : (lCountIn)))

	#define wcsncat_s(sDestInOut, lCountDestIn, sSrcIn, lCountIn)	wcsncat(			(sDestInOut), (sSrcIn), (lCountIn == _TRUNCATE ? (lCountDestIn - wcslen(sDestInOut) - 1) : (lCountIn)))
	#define strncat_s(sDestInOut, lCountDestIn, sSrcIn, lCountIn)	__builtin_strncat(	(sDestInOut), (sSrcIn), (lCountIn == _TRUNCATE ? (lCountDestIn - strlen(sDestInOut) - 1) : (lCountIn)))

	// both variants are supported by gcc
//	#define swprintf_s(sDestOut, lCountDestIn, args ...)			swprintf(			(sDestOut), ## args)
	#define swprintf_s(sDestOut, lCountDestIn, ...)					swprintf(			(sDestOut), __VA_ARGS__)
//	#define sprintf_s( sDestOut, lCountDestIn, args ...)			__builtin_sprintf(	(sDestOut), ## args)
	#define sprintf_s( sDestOut, lCountDestIn, ...)					__builtin_sprintf(	(sDestOut), __VA_ARGS__)

	//------------------------------------------------------------------------
	#if defined (PNDEV_UNICODE)
		typedef	wchar_t			_TCHAR;

		#define __T(x)			L ## x

		#undef	_TEXT
		#define _TEXT(x)		__T(x)

		#define __FUNCTION__	"xx"	// _TEXT(__FUNCTION__) -> L__FUNCTION__ which is not defined MINGW (possible solution: _TEXT("%hs"), __FUNCTION)

		#define	_tcscmp			wcscmp
		#define _tcslen			wcslen
		#define _tcsstr			wcsstr
		#define _tcschr			wcschr
		#define _tcsrchr		wcsrchr
		#define _tcstol			wcstol

		#define _tcscpy_s		wcscpy_s
		#define _tcscat_s		wcscat_s

		#define _tcsncpy_s		wcsncpy_s
		#define _tcsncat_s		wcsncat_s

		#define _tprintf		wprintf
		#define _stprintf_s		swprintf_s
	#else
		typedef	char			_TCHAR;

		#undef	_TEXT
		#define _TEXT(x)		x

		#define	_tcscmp			strcmp
		#define _tcslen			strlen
		#define _tcsstr			strstr
		#define _tcschr			strchr
		#define _tcsrchr		strrchr
		#define _tcstol			strtol

		#define _tcscpy_s		strcpy_s
		#define _tcscat_s		strcat_s

		#define _tcsncpy_s		strncpy_s
		#define _tcsncat_s		strncat_s

		#define _tprintf		printf
		#define _stprintf_s		sprintf_s
	#endif

	#endif

	//########################################################################
	//  Structures
	//########################################################################
	//------------------------------------------------------------------------

	//########################################################################
	//  Function prototypes
	//########################################################################

	// Note:
	//	- if all C files of user/driver include only overall IncludeFiles "common\xx_Inc.h" then compiler settings for character are always equal
	//		-> it doesn't matter if PNDEV_CHAR oder _TCHAR is used
	//	- if a single IncludeFile is used then character format depends on current settings if _TCHAR is used!
	//	- C files of common directory are always compiled correctly because of settings at precomp.h
	//		-> they can use _TCHAR / _txx()

#if defined (PNDEV_DRIVER_UM)
	BOOLEAN	fnPnDev_LoadDll(	const PNDEV_CHAR*	sPathDllIn,
								const UINT32		lDimErrorIn,
								void*				pFctPtrOut,
								HANDLE*				pHandleDllOut,
								PNDEV_CHAR*			sErrorOut);
	BOOLEAN	fnPnDev_UnloadDll(	HANDLE				hDllIn,
								const UINT32		lDimErrorIn,
								PNDEV_CHAR*			sErrorOut);
#endif

	BOOLEAN	fnSetThreadPrio(	const HANDLE		hThreadIn,
								const UINT32		lPrioThread,
								const INT32			lPolicyIn,
								const UINT32		lDimErrorIn,
								PNDEV_CHAR*			sErrorOut);
	UINT32	fnGetThreadPrio(	const HANDLE		hThreadIn,
								INT32*				pPolicyOut);
	HANDLE	fnGetThreadHandle(	void);
	BOOLEAN	fnCreateMutex(		const UINT32		lDimErrorIn,
								void*				pMutexOut,
								PNDEV_CHAR*			sErrorOut);
	BOOLEAN	fnDeleteMutex(		void*				pMutexIn,
								const UINT32		lDimErrorIn,
								PNDEV_CHAR*			sErrorOut);
	BOOLEAN	fnSetThreadPar(		const UINT32		lPrioThreadIn,
								const UINT32		lStackSizeThreadIn,
								const PNDEV_CHAR*	sNameThreadIn,
                                const UINT32		lDimErrorIn,
                                void*				pThreadParOut,
								PNDEV_CHAR*			sErrorOut);

	//########################################################################
	//------------------------------------------------------------------------
	#if defined (PNDEV_GCC)
	//------------------------------------------------------------------------
	errno_t _tcslwr_s(	PNDEV_CHAR*	sIn,										// convert a string to LowerCase
						size_t		lCountBufIn);
	errno_t _tcsupr_s(	PNDEV_CHAR*	sIn,										// convert a string to UpperCase
						size_t		lCountBufIn);
	#endif

	#if defined (PNDEV_DRIVER_UM)												// only used by UserCode
		//------------------------------------------------------------------------
		void	fnConvertStringCharToWchar(	const char*			sIn,
											wchar_t*			sResultOut);
		void	fnConvertStringCharToTchar(	const char*			sIn,
											PNDEV_CHAR*			sResultOut);
		//------------------------------------------------------------------------
		void	fnConvertStringWcharToChar(	const wchar_t*		sIn,
											char*				sResultOut);
		void	fnConvertStringWcharToTchar(const wchar_t*		sIn,
											PNDEV_CHAR*			sResultOut);
		//------------------------------------------------------------------------
		void	fnConvertStringTcharToWchar(const PNDEV_CHAR*	sIn,
											wchar_t*			sResultOut);
		//------------------------------------------------------------------------
	#endif

		//------------------------------------------------------------------------
		void	fnConvertStringTcharToChar(	const PNDEV_CHAR*	sIn,			// used by KernelCode and UserCode
											char*				sResultOut);
		//------------------------------------------------------------------------

	#ifdef __cplusplus		// if C++ compiler: End of C linkage
	}
	#endif

#endif
