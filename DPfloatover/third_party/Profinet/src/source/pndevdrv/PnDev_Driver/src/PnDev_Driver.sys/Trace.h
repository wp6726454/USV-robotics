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
/*  F i l e               &F: Trace.h                                   :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Debug tracing related definitions/macros
//
//****************************************************************************/
//
//  H i s t o r y :
//  ________________________________________________________________________
//
//  Date		Who				What
//  xxxx-xx-xx	Spichtinger		xx
/*****************************************************************************/

#include <evntrace.h> // For TRACE_LEVEL definitions

// If software tracing is defined in the sources file..
// WPP_DEFINE_CONTROL_GUID specifies the GUID used for this driver.
// *** REPLACE THE GUID WITH YOUR OWN UNIQUE ID ***
// WPP_DEFINE_BIT allows setting debug bit masks to selectively print.
// The names defined in the WPP_DEFINE_BIT call define the actual names
// that are used to control the level of tracing for the control guid
// specified.
//
// Name of the logger is PLX9x5x and the guid is
//   {CA630800-D4D4-4457-8983-DFBBFCAC5542}
//   (0xca630800, 0xd4d4, 0x4457, 0x89, 0x83, 0xdf, 0xbb, 0xfc, 0xac, 0x55, 0x42);
//

/* created by VS/Tools/Create GIUD
// {2C4E4946-3D2A-4565-AAA0-CBC08F3DD635}
DEFINE_GUID(<<name>>, 
			0x2c4e4946, 0x3d2a, 0x4565, 0xaa, 0xa0, 0xcb, 0xc0, 0x8f, 0x3d, 0xd6, 0x35);
*/

#define WPP_CHECK_FOR_NULL_STRING  //to prevent exceptions due to NULL strings

#define WPP_CONTROL_GUIDS \
	WPP_DEFINE_CONTROL_GUID(PnDevDrvTraceGuid, (2C4E4946, 3D2A, 4565, AAA0, CBC08F3DD635),	\
/*	WPP_DEFINE_CONTROL_GUID(PLX9x5xTraceGuid, (ca630800, D4D4, 4457,8983, DFBBFCAC5542),*/	\
		WPP_DEFINE_BIT(DBG_INIT)             /* bit  0 = 0x00000001 */ 						\
		WPP_DEFINE_BIT(DBG_PNP)              /* bit  1 = 0x00000002 */ 						\
		WPP_DEFINE_BIT(DBG_POWER)            /* bit  2 = 0x00000004 */ 						\
		WPP_DEFINE_BIT(DBG_WMI)              /* bit  3 = 0x00000008 */ 						\
		WPP_DEFINE_BIT(DBG_CREATE_CLOSE)     /* bit  4 = 0x00000010 */ 						\
		WPP_DEFINE_BIT(DBG_IOCTLS)           /* bit  5 = 0x00000020 */ 						\
		WPP_DEFINE_BIT(DBG_WRITE)            /* bit  6 = 0x00000040 */ 						\
		WPP_DEFINE_BIT(DBG_READ)             /* bit  7 = 0x00000080 */ 						\
		WPP_DEFINE_BIT(DBG_DPC)              /* bit  8 = 0x00000100 */ 						\
		WPP_DEFINE_BIT(DBG_INTERRUPT)        /* bit  9 = 0x00000200 */ 						\
		WPP_DEFINE_BIT(DBG_LOCKS)            /* bit 10 = 0x00000400 */ 						\
		WPP_DEFINE_BIT(DBG_QUEUEING)         /* bit 11 = 0x00000800 */ 						\
		WPP_DEFINE_BIT(DBG_HW_ACCESS)        /* bit 12 = 0x00001000 */ 						\
		/* You can have up to 32 defines. If you want more than that,						\
			you have to provide another trace control GUID */								\
		)


#define WPP_LEVEL_FLAGS_LOGGER(lvl,flags) WPP_LEVEL_LOGGER(flags)
#define WPP_LEVEL_FLAGS_ENABLED(lvl, flags) (WPP_LEVEL_ENABLED(flags) && WPP_CONTROL(WPP_BIT_ ## flags).Level  >= lvl)

//
// This comment block is scanned by the trace preprocessor to define our
// Trace function.
//
// begin_wpp config
// FUNC Trace{FLAG=DBG_INIT}(LEVEL, MSG, ...);
// FUNC TraceEvents(LEVEL, FLAGS, MSG, ...);
// end_wpp
//



