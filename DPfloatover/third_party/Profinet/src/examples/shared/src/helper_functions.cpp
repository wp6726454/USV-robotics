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
/*  F i l e               &F: helper_functions.cpp                      :F&  */
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
#include "eps_sys.h"

#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
#include <winsock2.h>
#include <iphlpapi.h>
#include <conio.h>
#endif

#if defined (EPS_CFG_USE_PNDEVDRV)
#include <precomp.h>
#include <PnDev_Driver_Inc.h> /* PnDevDrv Interface   */
#endif

#include <stdio.h>
#include <stdlib.h>

#include "eps_cp_hw.h"
#include "eps_pn_drv_if.h"

#include "servusrx.h"

#include "pnd_sys.h"

#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
#include "pcap.h"
#include <packet32_inc.h>      /* EDDS Lower Layer Intel Driver            */
#include "packet32.h"
#endif

#if (EPS_PLF == EPS_PLF_LINUX_X86)
#include "eps_pndevdrv.h"
#include <semaphore.h>
#endif

#include <eps_plf.h>
#include <eps_app.h>       

#ifdef _MSC_VER
#pragma comment(lib, "IPHLPAPI.lib")
#endif

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

#define CHAR_ESC   27

#define CHAR_BACK   8

#define MAX_EVENT_SIZE 3
static sem_t Events[MAX_EVENT_SIZE];

void getNetworkAdapterSelection(PNIO_CP_ID_TYPE* pCpId, PNIO_CP_ID_PTR_TYPE pListOfCps, PNIO_UINT8 nrOfCp)
{
    PNIO_UINT8 i;
    PNIO_CP_ID_PTR_TYPE pCpList = pListOfCps;
    PNIO_CP_ID_PTR_TYPE pDevice;
    int cmd = 777;

    if(nrOfCp == 0)
    {
        printf("\nNo network adapter found!");
        return;
    }

    printf("\r\nFound network adapters:\r\n");
    printf("----------------------------------------");
    
    for(i=0; i<nrOfCp; i++)
    {
        pDevice = (PNIO_CP_ID_PTR_TYPE)&pCpList[i];
        
        if( pDevice->CpSelection == PNIO_CP_SELECT_WITH_MAC_ADDRESS )
        {
            printf("\r\n %d ... %02x:%02x:%02x:%02x:%02x:%02x  -  %s", i,
                                                                   pDevice->CpMacAddr[0],
                                                                   pDevice->CpMacAddr[1],
                                                                   pDevice->CpMacAddr[2],
                                                                   pDevice->CpMacAddr[3],
                                                                   pDevice->CpMacAddr[4],
                                                                   pDevice->CpMacAddr[5],
                                                                   pDevice->Description);
        }
        else if ( pDevice->CpSelection == PNIO_CP_SELECT_WITH_PCI_LOCATION )
        {
            printf("\r\n %d ... Bus %2d, Dev %2d, Fct %2d - %s", i, pDevice->CpPciLocation.BusNr, 
                                                                    pDevice->CpPciLocation.DeviceNr, 
                                                                    pDevice->CpPciLocation.FunctionNr, 
                                                                    pDevice->Description);
        }
    }

    printf("\r\n----------------------------------------\r\n");
    
    
    while (cmd<0 || cmd>=nrOfCp)
    {
        printf("\nEnter the number of the network adapter: ");
        scanf_s("%d", &cmd);
    }

    memcpy( pCpId, &pCpList[cmd], sizeof(PNIO_CP_ID_TYPE) );

}

void allocateEvent(PNIO_UINT16 eventId)
{
    #if (EPS_PLF == EPS_PLF_WINDOWS_X86)
    Events[eventId] = CreateEvent( NULL, LSA_FALSE, LSA_FALSE, NULL );
    #elif (EPS_PLF == EPS_PLF_LINUX_X86)
    sem_init(&Events[eventId],0,0);
    #endif
}

void freeEvent(PNIO_UINT16 eventId)
{
    #if (EPS_PLF == EPS_PLF_WINDOWS_X86)
    CloseHandle(Events[eventId]);
    #elif (EPS_PLF == EPS_PLF_LINUX_X86)
    sem_destroy(&Events[eventId]);
    #endif
}

void setEvent(PNIO_UINT16 eventId)
{
    #if (EPS_PLF == EPS_PLF_WINDOWS_X86)
    SetEvent(Events[eventId]);
    #elif (EPS_PLF == EPS_PLF_LINUX_X86)
    sem_post(&Events[eventId]);
    #endif
}

void resetEvent(PNIO_UINT16 eventId)
{
    #if (EPS_PLF == EPS_PLF_WINDOWS_X86)
    ResetEvent(Events[eventId]);
    #elif (EPS_PLF == EPS_PLF_LINUX_X86)
    sem_destroy(&Events[eventId]);
    sem_init(&Events[eventId],0,0);
    #endif
}

void waitEvent(PNIO_UINT16 eventId)
{
    #if (EPS_PLF == EPS_PLF_WINDOWS_X86)
    WaitForSingleObject( Events[eventId], INFINITE );
    #elif (EPS_PLF == EPS_PLF_LINUX_X86)
    sem_wait(&Events[eventId]);
    #endif
}

int isCharEnter(char ch)
{
    return strchr("\r\n", ch) ? 1 : 0;
}

char *getStringFromStdio(char *pBuffer, int bufferLength)
{
    char ch;
    int i = 0;

    /* Read in single line from "stdin": */
	while((ch = (char)getchar()) != EOF)
	{
		if(isCharEnter(ch))
		{
			if(i == 0)
			{
				continue;  //consume ENTER at the beginning
			}
			else
			{
				break;  //end of text
			}
		}
        else if(ch == CHAR_BACK)
        {
            if(i > 0)
            {
                i--;
            }
            continue;
        } 
        else if(ch == CHAR_ESC)
        {
            i = 0;
            break;
        } 
        else if(i < (bufferLength - 1))
        {
             pBuffer[i] = ch;
             i++;
        }			
	}

    /* Terminate string with null character: */
    pBuffer[i] = '\0';
    printf("\n");

    return pBuffer;
}

void SWAP_U16( PNIO_UINT16  *pVar ) /* correct alignment! */
{
	*pVar = ((( (*pVar)) & 0xFF00L) >>  8) + ((( (*pVar)) & 0x00FFL) << 8);
}

void SWAP_U32( PNIO_UINT32  * pVar ) /* correct alignment! */
{
	PNIO_UINT32 temp;

	temp = *pVar;
	*pVar = (temp >> 24)
		| ((temp >> 8) & 0x0000ff00)
		| ((temp << 8) & 0x00ff0000)
		| (temp << 24);
}

#if (EPS_PLF == EPS_PLF_WINDOWS_X86)

static HANDLE s_ConsoleEvent;

void initConsoleLock(void)
{
    s_ConsoleEvent = CreateEvent( NULL, LSA_FALSE, LSA_FALSE, NULL );
}

void lockConsole(void)
{
    ResetEvent(s_ConsoleEvent);
}

void unlockConsole(void)
{
    SetEvent(s_ConsoleEvent);
}

void waitForConsole()
{
    WaitForSingleObject( s_ConsoleEvent, 1000 );
}
#endif
/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
