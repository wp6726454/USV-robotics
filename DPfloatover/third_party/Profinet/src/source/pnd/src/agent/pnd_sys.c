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
/*  C o m p o n e n t     &C: PnDriver                                  :C&  */
/*                                                                           */
/*  F i l e               &F: pnd_sys.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements the PnDriver system adaption for EPS (mem, locks, timer,...)  */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   1111
#define PND_MODULE_ID       1111

#include "pnd_int.h"
#include "pnd_sys.h"

#include "psi_int.h"

#include "eps_rtos.h"   // EPS adaption to OS

#include "eps_sys.h"    // EPS framework for locks, events
#include "eps_timer.h"  // EPS framework for timer

PND_FILE_SYSTEM_EXTENSION(PND_MODULE_ID)


/*---------------------------------------------------------------------------*/
LSA_UINT32 pnd_hton32( LSA_UINT32 host_long )
{
    return (PSI_HTON32(host_long));
}

LSA_UINT16 pnd_hton16( LSA_UINT16 host_short )
{
	return (PSI_HTON16(host_short));
}

/*---------------------------------------------------------------------------*/
/* Semaphore / Wait Events                                                   */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
LSA_VOID pnd_wait_event( LSA_UINT16 event_id  )
{
	// Wait, till event is set
	// Note: reset must be done before 
	
	eps_wait_event( event_id, PND_EVENT_NO_TIMEOUT );

}


/*---------------------------------------------------------------------------*/
/* Memory allocation                                                         */
/*---------------------------------------------------------------------------*/

LSA_VOID_PTR_TYPE pnd_mem_alloc( LSA_UINT32 nSize )
{
	return ( eps_mem_alloc( nSize, 0, 0 ));
}

/*---------------------------------------------------------------------------*/
LSA_VOID pnd_mem_free( LSA_VOID_PTR_TYPE pBuffer )
{
	eps_mem_free( pBuffer, 0, 0 );
}

/*---------------------------------------------------------------------------*/
/* string functions                                                          */
/*---------------------------------------------------------------------------*/
LSA_INT  pnd_strcmp (const LSA_UINT8 *str1, const LSA_UINT8 *str2)
{
    PND_ASSERT(str1);
    PND_ASSERT(str2);
	return eps_strcmp (str1, str2);
}

/*---------------------------------------------------------------------------*/
/* system adaptation for specific platform only !!                           */
/*---------------------------------------------------------------------------*/
LSA_INT pnd_strlen(const LSA_CHAR *str)
{
    PND_ASSERT(str);
	return( strlen(str));
}

LSA_INT    pnd_strncmp(const LSA_CHAR *s1, const LSA_CHAR *s2, LSA_INT n)
{
    PND_ASSERT(s1);
    PND_ASSERT(s2);
	return( strncmp(s1, s2, n));
}

/*---------------------------------------------------------------------------*/
/* Byte Swap                                                                 */
/*---------------------------------------------------------------------------*/

LSA_VOID   PND_SWAP_U16(
	LSA_UINT16  * short_ptr ) /* correct alignment! */
{
	LSA_UINT16 temp;

	temp = *short_ptr;
	*short_ptr = (temp >> 8) | (temp << 8);
}

LSA_VOID   PND_SWAP_U32(
	LSA_UINT32  * long_ptr ) /* correct alignment! */
{
	LSA_UINT32 temp;

	temp = *long_ptr;
	*long_ptr = (temp >> 24)
		| ((temp >> 8) & 0x0000ff00)
		| ((temp << 8) & 0x00ff0000)
		| (temp << 24);
}

/*---------------------------------------------------------------------------*/
/* FATAL ERROR                                                               */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/

LSA_VOID pnd_fatal(const LSA_CHAR *file, LSA_INT line, const LSA_CHAR *func, const LSA_CHAR *str)
{
	eps_fatal((LSA_CHAR *)file, line, (LSA_CHAR *)func, (LSA_CHAR *)str );
}

/*---------------------------------------------------------------------------*/

LSA_BOOL PND_FATAL_ERROR_OCCURED(LSA_VOID)
{
	return (g_pEpsData->bEpsInFatal); // eps global structure
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/


/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

