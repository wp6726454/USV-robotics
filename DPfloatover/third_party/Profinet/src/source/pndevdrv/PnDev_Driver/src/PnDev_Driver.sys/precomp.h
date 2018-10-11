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
/*  F i l e               &F: precomp.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Precompiled header file
//
//****************************************************************************/
//
//  H i s t o r y :
//  ________________________________________________________________________
//
//  Date		Who				What
//  xxxx-xx-xx	Spichtinger		xx
/*****************************************************************************/

#ifndef __precomp_h__
#define __precomp_h__

//------------------------------------------------------------------------
#if defined(PNDEV_OS_WIN)
//------------------------------------------------------------------------
#ifndef UNICODE
#define UNICODE
#define _UNICODE
#endif

#define PFD_MEM_ATTR \
  __in __drv_aliasesMem  // because of warning of PREfast for drivers (PFD)
// (indicates that the called function saves the value of the parameter and
// frees it later)
#define WIN9X_COMPAT_SPINLOCK

#include <ntddk.h>

#pragma warning(disable : 4201)  // nameless struct/union warning
#include <stdarg.h>
#include <wdf.h>
#pragma warning(default : 4201)

#include <Ntstrsafe.h>  // safe CRT functions
#include <initguid.h>   // required for GUID definitions
#include <tchar.h>      // _TCHAR, _tcslen, ...
#include <wdmguid.h>    // required for WMILIB_CONTEXT

//------------------------------------------------------------------------
#elif defined(PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
#define PFD_MEM_ATTR

#include <Adonis_Inc.h>
#include <acpi_drv.h>
#include <memory_management.h>
#include <mman.h>
#include <pci.h>
#include <time.h>

//------------------------------------------------------------------------
#elif defined(PNDEV_OS_LINUX)
//------------------------------------------------------------------------
#define PFD_MEM_ATTR  // required for Basic.h

#if defined PNDEV_DRIVER_SYS
#include <linux/cdev.h>
#include <linux/compat.h> /* 32-Bit compatibility */
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/eventfd.h> /* eventfd  */
#include <linux/fdtable.h> /* for sending events with eventfd in kernel */
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/interrupt.h> /* pci interrupts */
#include <linux/io.h>
#include <linux/ioctl.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/pci.h>
#include <linux/pid.h>      /* pid for sending events with eventfd in kernel */
#include <linux/rcupdate.h> /* locking when looking up user eventfd */
#include <linux/sched.h> /* current->pid for sending events with eventfd in kernel */
#include <linux/slab.h>
#include <linux/time.h>  /* for time in systhread */
#include <linux/timer.h> /* Timer for systhread */
#include <linux/uaccess.h>
#include <linux/vmalloc.h> /**/
#endif

//------------------------------------------------------------------------
#elif defined(PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

#endif
