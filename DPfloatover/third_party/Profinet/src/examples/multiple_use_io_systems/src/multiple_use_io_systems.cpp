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
/*  F i l e               &F: multiple_use_io_systems.cpp               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*    This example shows how to use the feature multiple use IO systems.     */
/*                                                                           */
/*    After completing the startup process IP Suite and Name Of Station of   */
/*  the PN Driver can be configured. In this example it is requested to      */
/*  store the IP Suite and NoS values remanenetly in a file named “rema.xml” */ 
/*  by handling the PNIO_CBE_REMA_READ_CONF callback. This leads to that a   */
/*  next start of the application the values for IP Suite and Name Of        */ 
/*  Station are taken from the “rema.xml” file.                              */
/*                                                                           */
/*  IP      : 192.168.0.120                                                  */
/*  NetMask : 255.255.255.0                                                  */
/*  Gateway : 0.0.0.0                                                        */
/*  NoS     : pndriver                                                       */
/*                                                                           */
/*****************************************************************************/
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <psi_cfg.h>

#define LTRC_ACT_MODUL_ID   0000
#define PND_MODULE_ID       0000

#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
#include <crtdbg.h>
#endif

#include "pnd_int.h"
#include "pniobase.h"
#include "servusrx.h"
#include <stdio.h>
#include <string.h>
#include "helper_functions.h"
#include "user_application_common.h"

extern "C" 
{
    extern PNIO_UINT32 g_ApplHandle;
}
extern FILE* pTraceFile;
void callback_for_ifc_record_read_conf(PNIO_CBE_PRM*) {}
void callback_for_alarm_ind(PNIO_CBE_PRM* ) {}
void callback_for_ds_read_conf(PNIO_CBE_PRM*) {}
void callback_for_ds_write_conf(PNIO_CBE_PRM*) {}

void callback_for_ifc_set_ip_and_nos( PNIO_CBE_PRM * pCbfPrm )
{
    if(pCbfPrm->SetAddrConf.Err == PNIO_OK)    
    {
        printf("\nIP Suite and NoS have been set correctly.\n");
    }
    else
    {
        printf("\nSetting IP Suite and NoS failed.\n");
    }
}

void callback_for_ifc_rema_read( PNIO_CBE_PRM * pCbfPrm )
{
	#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
	waitForConsole();
    #endif 
	
    FILE* remaFile = fopen("rema.xml","w+");

    if(remaFile != NULL)
    {
        fwrite(pCbfPrm->RemaReadConf.RemaXMLBuffer,1,pCbfPrm->RemaReadConf.RemaXMLBufferLength,remaFile);
		
        fflush(remaFile);
        fclose(remaFile);

        printf("\nrema.xml file has been created/updated\n");
    }
    else
    {
        printf("\nrema.xml file could not be opened\n");
    }
}

void tailorIPSuiteAndNos()
{
    PNIO_IPv4 ipSuite;
    PNIO_NOS deviceName;
    PNIO_UINT8 mode;
    PNIO_UINT32 result;

    ipSuite.IpAddress[0] = 192;
    ipSuite.IpAddress[1] = 168;
    ipSuite.IpAddress[2] = 0;
    ipSuite.IpAddress[3] = 120;

    ipSuite.NetMask[0] = 255;
    ipSuite.NetMask[1] = 255;
    ipSuite.NetMask[2] = 255;
    ipSuite.NetMask[3] = 0;

    ipSuite.Gateway[0] = 0;
    ipSuite.Gateway[1] = 0;
    ipSuite.Gateway[2] = 0;
    ipSuite.Gateway[3] = 0;

    ipSuite.Remanent = PNIO_TRUE;

    mode = PNIO_SET_IP_MODE | PNIO_SET_NOS_MODE;

    deviceName.Length = 8;
    memcpy(&deviceName.Nos[0], "pndriver", deviceName.Length);
    deviceName.Nos[deviceName.Length] = 0;

    deviceName.Remanent = PNIO_TRUE ;
    
    result = PNIO_interface_set_ip_and_nos(g_ApplHandle,                                      
                                      (PNIO_SET_IP_NOS_MODE_TYPE) mode,
                                      ipSuite,
                                      deviceName);

    if(result != PNIO_OK)
    {
        printf("PNIO_ifc_set_ip_and_nos() returned error: %s\n", getPnioError(result));
    }
}

int main()
{
    int  cmd = 777;
    PNIO_UINT32 result;
    #if (EPS_PLF == EPS_PLF_WINDOWS_X86)
    _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
    initConsoleLock();
    #endif

    initPnd();

    while (cmd != 0)
    {
        #if (EPS_PLF == EPS_PLF_WINDOWS_X86)
        lockConsole();
        #endif
        printf("\n");
        printf("PROFINET Driver | Multiple Use IO Systems user application  \n");
        printf("================================\n");
        printf("1...Startup                     \n");
        printf("2...Start tailoring process     \n" );
        printf("0...Exit                        \n");
        
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
                startUpPnd("PNDriverBase_MultipleUseIoSys/Station_1.PN Driver_1.PNDriverConfiguration.xml", "rema.xml");                

                if( (result = PNIO_interface_open(1, callback_for_ifc_record_read_conf ,callback_for_alarm_ind , &g_ApplHandle)) != PNIO_OK)
                {
                    printf("PNIO_interface_open() returned error:%s\n", getPnioError(result));
                }
                else if( (result = PNIO_controller_open(1, PNIO_CEP_MODE_CTRL,callback_for_ds_read_conf ,callback_for_ds_write_conf ,callback_for_alarm_ind , &g_ApplHandle)) != PNIO_OK)
                {
                    printf("PNIO_controller_open() returned error: %s\n", getPnioError(result));
                }
                else if( (result = PNIO_interface_register_cbf( g_ApplHandle, PNIO_CBE_IFC_SET_ADDR_CONF, callback_for_ifc_set_ip_and_nos)) != PNIO_OK)
                {
                    printf("PNIO_interface_register_cbf() returned error: %s\n", getPnioError(result));
                }
                else if( (result = PNIO_interface_register_cbf( g_ApplHandle, PNIO_CBE_REMA_READ_CONF, callback_for_ifc_rema_read)) != PNIO_OK)
                {
                    printf("PNIO_interface_register_cbf() returned error: %s\n", getPnioError(result));
                }
                else if( (result = PNIO_set_mode(g_ApplHandle,PNIO_MODE_OPERATE)) == PNIO_OK)
                {
                    printf("Mode has been set to operate\n");
                }
                else
                {
                    printf("PNIO_set_mode() returned error: %s\n", getPnioError(result));
                }
                break;
            case 2: 
                tailorIPSuiteAndNos();
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

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/