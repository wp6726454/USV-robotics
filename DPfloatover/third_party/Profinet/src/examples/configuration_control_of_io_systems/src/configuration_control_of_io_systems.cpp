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
/*  F i l e               &F: configuration_control_of_io_systems.cpp   :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*    This example shows how to use the feature configuration control of IO  */
/*  systems. At the startup one of the two possible configurations can be    */
/*  choosen. Each configuration uses a master project and a generated        */
/*  hardware configuration.                                                  */
/*                                                                           */
/*    - Configuration 1: optional IODevices.                                 */
/*    - Configuration 2: optional IODevices and programmale peers.           */
/*                                                                           */
/*    In order to set the preferred configuration without receiving          */
/*  diagnosis alarms first deactivate all devices and then execute the       */
/*  tailoring process.                                                       */
/*                                                                           */
/*****************************************************************************/
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <stdio.h>
#include <psi_cfg.h>

#define LTRC_ACT_MODUL_ID   0000
#define PND_MODULE_ID       0000

#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
#include <crtdbg.h>
#endif

#include "pnd_int.h"
#include "pniobase.h"
#include "servusrx.h"
#include "helper_functions.h"
#include "user_application_common.h"

extern "C" 
{
    extern PNIO_UINT32 g_ApplHandle;
}
extern FILE* pTraceFile;
void callback_for_ifc_record_read_conf(PNIO_CBE_PRM*) {}
void callback_for_ds_read_conf(PNIO_CBE_PRM*) {}
void callback_for_ds_write_conf(PNIO_CBE_PRM*) {}

void callback_for_alarm_ind(PNIO_CBE_PRM* pCbfPrm)
{
    #if (EPS_PLF == EPS_PLF_WINDOWS_X86)
    waitForConsole();
    #endif   

    /* Check if correct callback type */
    if(pCbfPrm->CbeType == PNIO_CBE_ALARM_IND || pCbfPrm->CbeType == PNIO_CBE_IFC_ALARM_IND)
    {
        if(pCbfPrm->AlarmInd.pAlarmData->AlarmType == PNIO_ALARM_DEV_RETURN)
        {
            printf("PNIO_ALARM_DEV_RETURN has been received for device %d\n", pCbfPrm->AlarmInd.pAlarmData->DeviceNum);
        }
    }
}

void callback_for_iosystem_reconfig( PNIO_CBE_PRM * pCbfPrm )   
{
	#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
    waitForConsole();
    #endif 

	if( pCbfPrm->IoSystemReconf.Status != PNIO_OK )
	{
		printf("Device activation/deactivation failed: %s\n", getPnioError((int)pCbfPrm->IoSystemReconf.Status));
	}
    else
    {
        switch (pCbfPrm->IoSystemReconf.Mode) 
        {
        case PNIO_DA_TRUE:
            printf("All configured devices have been activated \n");           
            break;
        case PNIO_DA_FALSE:
            printf("All devices have been deactivated \n");
            break;
        default:
            break;
        };
    }
}

void prepareConfiguration(int configuration, PNIO_UINT32& deviceCnt, PNIO_ADDR* deviceList, 
                             PNIO_UINT32& portInterconCnt, PNIO_ADDR* pPortInterConList)
{
	switch(configuration)
	{
	case 1:
		deviceCnt = 1;
		portInterconCnt = 0;
		
		deviceList[0].AddrType = PNIO_ADDR_LOG;
		deviceList[0].u.Addr = 268;		
		break;
	case 2:
		deviceCnt = 0;		
		portInterconCnt = 5;
		
		pPortInterConList[0].u.Addr = 65;   
		pPortInterConList[1].u.Addr = 260;

		pPortInterConList[2].u.Addr = 261;   
		pPortInterConList[3].u.Addr = 273;

		pPortInterConList[4].u.Addr = 274;   
		pPortInterConList[5].u.Addr = 283;

		pPortInterConList[6].u.Addr = 284;  
		pPortInterConList[7].u.Addr = 293;
		
		pPortInterConList[8].u.Addr = 294;  
		pPortInterConList[9].u.Addr = 303;	
		break;
	default:
		printf("Wrong configuration number\n");
		break;		
	}
}

int main()
{    
    int cmd = 777;
    PNIO_UINT32 result;
    int configuration = 0;
    PNIO_ADDR deviceList[3];
    PNIO_ADDR portInterconList[10];
    PNIO_UINT32 deviceCnt = 0;
    PNIO_UINT32 portInterconCnt = 0;

    initPnd();

    #if (EPS_PLF == EPS_PLF_WINDOWS_X86)
    _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
    initConsoleLock();
    #endif

    while (cmd != 0)
    {
        #if (EPS_PLF == EPS_PLF_WINDOWS_X86)
        lockConsole();
        #endif
        printf("\n");
        printf("PROFINET Driver | Configuration control of IO systems\n");
        printf("===========================================\n");
        printf("1...Startup                                \n");
        printf("2...Deactivate all IO devices              \n");
        printf("3...Execute tailoring process              \n");
        printf("0...Exit                                   \n");

        #if (EPS_PLF == EPS_PLF_WINDOWS_X86)
        unlockConsole();
        scanf_s("%d", &cmd);
        #else
        scanf("%d", &cmd);
        #endif

        while(getchar()!='\n');
        switch(cmd) 
        {
        case 1:
            printf("Please enter configuration number (1 or 2)\n");
            #if (EPS_PLF == EPS_PLF_WINDOWS_X86)
            unlockConsole();
            scanf_s("%d", &configuration);
            #else
            scanf("%d", &configuration);
            #endif

            switch(configuration)
            {
            case 1:
                startUpPnd("PNDriverBase_OptIODevices/Station_1.PN Driver_1.PNDriverConfiguration.xml", NULL);
                break;
            case 2:
                startUpPnd("PNDriverBase_FlexibleTopology/Station_1.PN Driver_1.PNDriverConfiguration.xml", NULL);
                break;
            default:
                configuration = 0;
                printf("Configuration number has to be 1 or 2\n");
                break;
            }

            if(configuration != 0)
            {
                if( (result = PNIO_interface_open(1, callback_for_ifc_record_read_conf, callback_for_alarm_ind, &g_ApplHandle)) != PNIO_OK)
                {
                    printf("PNIO_interface_open() returned error: %s\n", getPnioError(result));
                }
                else if((result = PNIO_controller_open(1, PNIO_CEP_MODE_CTRL, callback_for_ds_read_conf, callback_for_ds_write_conf, callback_for_alarm_ind, &g_ApplHandle)) != PNIO_OK)
                {
                    printf("PNIO_controller_open() returned error: %s\n", getPnioError(result));
                }
                else if((result = PNIO_register_cbf( g_ApplHandle, PNIO_CBE_IOSYSTEM_RECONFIG, callback_for_iosystem_reconfig)) != PNIO_OK)
                {
                    printf("PNIO_register_cbf() returned error: %s\n", getPnioError(result));
                }
                else if( (result = PNIO_set_mode(g_ApplHandle,PNIO_MODE_OPERATE)) != PNIO_OK)
                {
                    printf("PNIO_set_mode() returned error: %s\n", getPnioError(result));
                }
                else
                {
                    printf("Mode has been set to operate\n");
                }
            }
            break;
        case 2:
            result = PNIO_iosystem_reconfig(g_ApplHandle, PNIO_IOS_RECONFIG_MODE_DEACT, 0, PNIO_NULL, 0, PNIO_NULL);
            printf("PNIO_iosystem_reconfig returned result: %s at mode PNIO_IOS_RECONFIG_MODE_DEACT\n", getPnioError(result));
            break;
        case 3:
            prepareConfiguration(configuration, deviceCnt, deviceList, portInterconCnt, portInterconList);
            result = PNIO_iosystem_reconfig(g_ApplHandle,                                        
                                            PNIO_IOS_RECONFIG_MODE_TAILOR,
                                            deviceCnt,
                                            deviceList,
                                            portInterconCnt,
                                            portInterconList);

            printf("PNIO_iosystem_reconfig() returned result: %s at mode PNIO_IOS_RECONFIG_MODE_TAILOR\n", getPnioError(result));
            break;
        case 0:
            if ((result = PNIO_interface_close(g_ApplHandle)) != PNIO_OK)
            {
                printf("PNIO_interface_close() returned error: %s\n", getPnioError(result));
            }
            else if ((result = PNIO_controller_close(g_ApplHandle)) != PNIO_OK)
            {
                printf("PNIO_controller_close() returned error: %s\n", getPnioError(result));
            }
            else if((result = SERV_CP_shutdown()) != PNIO_OK)
            {
                printf("SERV_CP_shutdown() returned error: %s\n", getPnioError(result));
            }
            else
            {
                SERV_CP_undo_init();
            }
            break;
        default:
            break;
        }                   
    }

    if (pTraceFile)
    {
        fclose(pTraceFile);
    }
    pTraceFile = NULL;
}