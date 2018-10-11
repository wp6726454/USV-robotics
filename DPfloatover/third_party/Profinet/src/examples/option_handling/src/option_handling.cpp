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
/*  F i l e               &F: option_handling.cpp                       :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*    This example shows how to use the feature OptionHandling/Configuration */
/*  Control.                                                                 */
/*                                                                           */
/*    The configuration of the ET 200SP station comprises all modules of all */
/*  variants of the machine. A concrete variant is enabled by writing a      */
/*  control data record to the IODevice. The actual configuration of an      */
/*  IODeviceis read by using the feedback data record.                       */
/*                                                                           */
/*  Variant 1: Corresponds to the maximum configuration coming from XML      */
/*  configuration.                                                           */
/*                                                                           */
/*****************************************************************************/
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <stdio.h>
#include <psi_cfg.h>

#include "pniobase.h"
#include "servusrx.h"
#include "pniousrx.h"
#include "pnioerrx.h"

#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
#include <crtdbg.h>
#include <ole2.h>
#endif

#include "helper_functions.h"
#include "user_application_common.h"

#if (EPS_PLF == EPS_PLF_LINUX_X86)
#include <unistd.h>
#endif


extern "C" 
{
    extern PNIO_UINT32 g_ApplHandle;
}
extern FILE* pTraceFile;

#define MAX_NR_OF_RETRY 7     // 7-seconds
#define DEVICE_SUBMODULE_LADDR 258

typedef enum
{
    IDLE,
    FAILED,
    DONE    
} READ_REQ_STATUS_TYPE;

PNIO_UINT16 g_recordReadWriteEvent;
PNIO_UINT8 g_controlDataRecordBuffer[14] = {0};
READ_REQ_STATUS_TYPE g_readWriteReqStatus;

void displayFeedbackRecordResult(const PNIO_UINT8 *pBuffer, const PNIO_UINT8 *pControlDataRecordBuffer);

void callback_for_ifc_record_read_conf(PNIO_CBE_PRM*) {}

void callback_for_alarm_ind(PNIO_CBE_PRM*)
{}

void callback_for_ds_write_conf(PNIO_CBE_PRM* pCbfPrm)
{
    switch (pCbfPrm->RecReadConf.RecordIndex) 
    {
        case 0xC4: // Control Record

            if(pCbfPrm->RecReadConf.Err.AddValue1 != PNIO_OK)
            {
                g_readWriteReqStatus = FAILED;
            }
            else
            {
                g_readWriteReqStatus = DONE;
            }
            break;

        default:
            break;
    }

    setEvent(g_recordReadWriteEvent);
}

void callback_for_ds_read_conf(PNIO_CBE_PRM* pCbfPrm)
{

    switch (pCbfPrm->RecReadConf.RecordIndex) 
    {
        case 0xC5: // Feedback Record

            if(pCbfPrm->RecReadConf.Err.AddValue1 != PNIO_OK)
            {
                g_readWriteReqStatus = FAILED;
            }
            else
            {
                g_readWriteReqStatus = DONE;

                displayFeedbackRecordResult(pCbfPrm->RecReadConf.pBuffer, g_controlDataRecordBuffer);
            }
            break;

        default:
            break;
    }

    setEvent(g_recordReadWriteEvent);
}

void displayFeedbackRecordResult(const PNIO_UINT8 *pBuffer, const PNIO_UINT8 *pControlDataRecordBuffer)
{
    printf("DONE\n\n");

    printf("Parametrization can take longer depending on the IO-Device and its hardware configuration under certain circumstances.\n");
    printf("Please try again, if the attempt is not succesful.\n\n");

    if(pControlDataRecordBuffer[4] == 0)
    {
        printf("%s\n", pBuffer[4] == 0x01 ? 
             "A false module/BU cover is inserted" : 
             "M1 is not inserted in the real configuration");
    }
    else
    {
        printf("%s\n", pBuffer[4] == 0x01 ? 
            "M1 is correctly inserted in the real configuration" : 
            "M1 module is not inserted or a false module/BU cover is inserted");
    }

    if(pControlDataRecordBuffer[6] == 0)
    {
        printf("%s\n", pBuffer[6] == 0x01 ? 
            "A false module/BU cover is inserted" : 
            "M2 is not inserted in the real configuration");
    }
    else
    {
        printf("%s\n", pBuffer[6] == 0x01 ? 
            "M2 is correctly inserted in the real configuration" : 
            "M2 module is not inserted or a false module/BU cover is inserted");
    }

    if(pControlDataRecordBuffer[8] == 0)
    {
        printf("%s\n", pBuffer[8] == 0x01 ? 
            "A false module/BU cover is inserted" : 
            "M3 is not inserted in the real configuration");
    }
    else
    {
        printf("%s\n", pBuffer[8] == 0x01 ? 
            "M3 is correctly inserted in the real configuration" : 
            "M3 module is not inserted or a false module/BU cover is inserted");
    }

    if(pControlDataRecordBuffer[10] == 0)
    {
        printf("%s\n", pBuffer[10] == 0x01 ? 
            "A false module/BU cover is inserted" : 
            "M4 is not inserted in the real configuration");
    }
    else
    {
        printf("%s\n", pBuffer[10] == 0x01 ? 
            "M4 is correctly inserted in the real configuration" : 
            "M4 module is not inserted or a false module/BU cover is inserted");
    }

    if(pControlDataRecordBuffer[12] == 0)
    {
        printf("%s\n", pBuffer[12] == 0x01 ? 
            "A false module/BU cover is inserted" : 
            "SM is not inserted in the real configuration");
    }
    else
    {
        printf("%s\n", pBuffer[12] == 0x01 ? 
            "SM is correctly inserted in the real configuration" : 
            "SM module is not inserted or a false module/BU cover is inserted");
    }        
}

void buildControlDataRecord(PNIO_UINT32 variant, PNIO_UINT8 *pBuffer)
{

    // Header of Control Record
    pBuffer[0] = 0x0E; pBuffer[1] = 0xC4;
    pBuffer[2] = 0x02; pBuffer[3] = 0x00; 

    switch(variant)
    {
    // Variant 2: Module 2 and Module 3 are hidden.
    case 2:
        pBuffer[4]  = 0x01; pBuffer[5]  = 0x00; 
        pBuffer[6]  = 0x00; pBuffer[7]  = 0x00; 
        pBuffer[8]  = 0x00; pBuffer[9]  = 0x00; 
        pBuffer[10] = 0x02; pBuffer[11] = 0x00; 
        pBuffer[12] = 0x03; pBuffer[13] = 0x00;
        break;

    // Variant 3: 
    //   - The module (M1) configured for slot 1 is inserted in slot 1.
    //   - The module (M2) configured for slot 2 is inserted in slot 4.
    //   - The module (M3) configured for slot 3 is inserted in slot 2.
    //   - The module (M4) configured for slot 4 is inserted in slot 3.
    case 3: 
        pBuffer[4]  = 0x01; pBuffer[5]  = 0x00; 
        pBuffer[6]  = 0x04; pBuffer[7]  = 0x00; 
        pBuffer[8]  = 0x02; pBuffer[9]  = 0x00; 
        pBuffer[10] = 0x03; pBuffer[11] = 0x00; 
        pBuffer[12] = 0x05; pBuffer[13] = 0x00;
        break;

    // Variant 4: 
    //   - The module (M1) configured for slot 1 is inserted in slot 1.
    //   - The module (M2) configured for slot 2 is inserted in slot 2 or a BU cover is inserted in the real slot.
    //   - The module (M3) configured for slot 3 is inserted in slot 3.
    //   - The module (M4) configured for slot 4 is inserted in slot 4.
    case 4:
        pBuffer[4]  = 0x01; pBuffer[5]  = 0x00; 
        pBuffer[6]  = 0x82; pBuffer[7]  = 0x00; 
        pBuffer[8]  = 0x03; pBuffer[9]  = 0x00; 
        pBuffer[10] = 0x04; pBuffer[11] = 0x00; 
        pBuffer[12] = 0x05; pBuffer[13] = 0x00;
        break;

    // Variant 5:
    //   - The module (M1) configured for slot 1 is inserted in slot 1.
    //   - The module (M2) configured for slot 2 is inserted in slot 2. 
    //   - The module (M3) configured for slot 3 is inserted in slot 3. A new potential group is opened.
    //   - The module (M4) configured for slot 4 is inserted in slot 4.
    case 5:
        pBuffer[4]  = 0x01; pBuffer[5]  = 0x00; 
        pBuffer[6]  = 0x02; pBuffer[7]  = 0x00; 
        pBuffer[8]  = 0x03; pBuffer[9]  = 0x01; 
        pBuffer[10] = 0x04; pBuffer[11] = 0x00; 
        pBuffer[12] = 0x05; pBuffer[13] = 0x00;
        break;

    // Variant 6:
    //   - The module (M1) configured for slot 1 is inserted in slot 3. A new potential group is opened.
    //   - The module (M2) configured for slot 2 is not inserted.
    //   - The module (M3) configured for slot 3 is inserted in slot 1.
    //   - The module (M4) configured for slot 4 is inserted in slot 2 or a BU cover is inserted in slot 2.
    case 6:
        pBuffer[4]  = 0x03; pBuffer[5]  = 0x01; 
        pBuffer[6]  = 0x00; pBuffer[7]  = 0x00; 
        pBuffer[8]  = 0x01; pBuffer[9]  = 0x01; 
        pBuffer[10] = 0x82; pBuffer[11] = 0x00; 
        pBuffer[12] = 0x04; pBuffer[13] = 0x00;
        break;
    default:
        break;
    }
}

int main()
{
    int  cmd = 777;
    PNIO_UINT32 result;
    PNIO_ADDR laddr;
    PNIO_REF reqRef = 1;
    PNIO_UINT32 variant;
    PNIO_UINT32 length = sizeof(g_controlDataRecordBuffer);
    PNIO_UINT8 tryCount;

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
        printf("PROFINET Driver | Option Handling / Configuration Control \n");
        printf("========================================================= \n");
        printf("1...Startup                                               \n");
        printf("2...Enable Variant 2                                      \n");
        printf("3...Enable Variant 3                                      \n");
        printf("4...Enable Variant 4                                      \n");
        printf("5...Enable Variant 5                                      \n");
        printf("6...Enable Variant 6                                      \n");
        printf("0...Exit                                                  \n\n");
        
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
                startUpPnd("PNDriverBase_ConfigControlIODevices/Station_1.PN Driver_1.PNDriverConfiguration.xml", "rema.xml");

                if( (result = PNIO_interface_open(1, callback_for_ifc_record_read_conf, callback_for_alarm_ind, &g_ApplHandle)) != PNIO_OK)
                {
                    printf("PNIO_interface_open() returned error: %s\n", getPnioError(result));
                }
                else if((result = PNIO_controller_open(1, PNIO_CEP_MODE_CTRL, callback_for_ds_read_conf, callback_for_ds_write_conf, callback_for_alarm_ind, &g_ApplHandle)) != PNIO_OK)
                {
                    printf("PNIO_controller_open() returned error: %s\n", getPnioError(result));
                }
                else if((result = PNIO_set_mode(g_ApplHandle,PNIO_MODE_OPERATE)) != PNIO_OK)
                {
                    printf("PNIO_register_cbf() returned error: %s\n", getPnioError(result));
                }

                // Create Event
                g_recordReadWriteEvent = 0;
                allocateEvent(g_recordReadWriteEvent);
                break;

            case 2: // Enable Variant 2
            case 3: // Enable Variant 3
            case 4: // Enable Variant 4
            case 5: // Enable Variant 5
            case 6: // Enable Variant 6
                variant = cmd;
                tryCount = 0;
                g_readWriteReqStatus = IDLE;
                              
                // Reset Event
                resetEvent(g_recordReadWriteEvent);

                laddr.IODataType = PNIO_IO_IN;
                laddr.AddrType = PNIO_ADDR_LOG;
                laddr.u.Addr = DEVICE_SUBMODULE_LADDR;

                buildControlDataRecord(variant, g_controlDataRecordBuffer);

                printf("\nWriting variant %d to control data record...", variant);

                result = PNIO_rec_write_req(g_ApplHandle, &laddr, 0xC4 /* Control Data Record */, reqRef, sizeof(g_controlDataRecordBuffer), g_controlDataRecordBuffer);

                if(result != PNIO_OK)
                {
                    printf("\nPNIO_rec_write_req() returned error: %s\n", getPnioError(result));
                    break;
                }

                // Wait Event
                waitEvent(g_recordReadWriteEvent);

                if(g_readWriteReqStatus == FAILED)
                {
                    printf("\nPNIO_rec_write_req() failed.\n");
                    break;
                }
                                
                printf("DONE\n");
                
                g_readWriteReqStatus = IDLE;

                printf("Reading feedback data record...");

                while(g_readWriteReqStatus != DONE)
                {
                    #if (EPS_PLF == EPS_PLF_WINDOWS_X86)
                    // 1sec wait
                    Sleep(1000);
                    #elif (EPS_PLF == EPS_PLF_LINUX_X86)
                    // 1sec wait
                    sleep(1);
                    #endif
                    
                    // Reset Event
                    resetEvent(g_recordReadWriteEvent);
                    
                    result = PNIO_rec_read_req(g_ApplHandle, &laddr, 0xC5 /* Feedback Record */, reqRef, length);

                    if(result != PNIO_OK)
                    {
                        printf("\nPNIO_rec_read_req() returned error: %s\n", getPnioError(result));
                        break;
                    }

                    // Wait Event
                    waitEvent(g_recordReadWriteEvent);

                    if(tryCount++ >= MAX_NR_OF_RETRY)
                    {
                        printf("\nReading feedback record failed. No response from the device.\n");
                        break;
                    }
                }
                break;

            case 0: // Exit
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

                // Free Event
                freeEvent(g_recordReadWriteEvent);
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