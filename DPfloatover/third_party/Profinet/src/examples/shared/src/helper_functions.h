#ifndef HELPERFUNCTIONS_H
#define HELPERFUNCTIONS_H

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
/*  F i l e               &F: helper_functions.h                        :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*****************************************************************************/
#include "pniobase.h"
#include "servusrx.h"

void getNetworkAdapterSelection(PNIO_CP_ID_TYPE* pCpId, PNIO_CP_ID_PTR_TYPE pListOfCps, PNIO_UINT8 nrOfCp);

#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
void initConsoleLock(void);
void lockConsole(void);
void unlockConsole(void);
void waitForConsole(void);
#endif

void allocateEvent(PNIO_UINT16 eventId);
void freeEvent(PNIO_UINT16 eventId);
void setEvent(PNIO_UINT16 eventId);
void resetEvent(PNIO_UINT16 eventId);
void waitEvent(PNIO_UINT16 eventId);

void SWAP_U16( PNIO_UINT16  *pVar );
void SWAP_U32( PNIO_UINT32  * pVar );

char *getStringFromStdio(char *pBuffer, int bufferLength);

#endif /*HELPERFUNCTIONS_H*/