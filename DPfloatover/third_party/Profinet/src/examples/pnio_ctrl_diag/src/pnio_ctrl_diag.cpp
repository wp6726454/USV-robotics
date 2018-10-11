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
/*  F i l e               &F: pnio_ctrl_diag.cpp                        :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*    This example shows how to gather configuration and diagnosis           */
/*  information of devices which are coupled to a controller interface by    */
/*  using the PNIO_ctrl_diag() and the PNIO_rec_read_req() functions of the  */
/*  IOBaseAPI. Besides the device information, this function also provides   */
/*  information about the network parameters of the local IO interface.      */
/*                                                                           */
/*****************************************************************************/
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <psi_cfg.h>

#include "pniobase.h"
#include "servusrx.h"
#include "pniousrx.h"
#include "pnioerrx.h"

#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
#include <crtdbg.h>
#endif

#include "helper_functions.h"
#include "user_application_common.h"
#include "diagnosis_helper.h"
#include "im_data.h"

extern "C" 
{
    extern PNIO_UINT32 g_ApplHandle;
}
extern FILE* pTraceFile;

DiagnosisHelper diagnosisHelper;
PNIO_UINT16 g_diagReqWaitEvent;
PNIO_UINT16 g_setIMValuesEvent;

void callback_for_ifc_record_read_conf(PNIO_CBE_PRM*) {}

void callback_for_alarm_ind(PNIO_CBE_PRM* ) {}

void callback_for_ds_write_conf(PNIO_CBE_PRM* pCbfPrm)
{
    // Storing the result of the record write operation
    diagnosisHelper.setErrorCode(pCbfPrm->RecWriteConf.Err.AddValue1);

    setEvent(g_setIMValuesEvent);
}

void callback_for_ds_read_conf(PNIO_CBE_PRM* pCbfPrm)
{
    // Storing the result of the record read operation
    diagnosisHelper.setErrorCode(pCbfPrm->RecReadConf.Err.AddValue1);

    switch (pCbfPrm->RecReadConf.RecordIndex) 
    {
        case IM0_TYPE:
            diagnosisHelper.extractIMData(pCbfPrm->RecReadConf.pBuffer, IM0_TYPE);
            break;
        case IM1_TYPE:
            diagnosisHelper.extractIMData(pCbfPrm->RecReadConf.pBuffer, IM1_TYPE);
            break;
        case IM2_TYPE:
            diagnosisHelper.extractIMData(pCbfPrm->RecReadConf.pBuffer, IM2_TYPE);
            break;
        case IM3_TYPE:
            diagnosisHelper.extractIMData(pCbfPrm->RecReadConf.pBuffer, IM3_TYPE);
            break;
        case IM4_TYPE:
            diagnosisHelper.extractIMData(pCbfPrm->RecReadConf.pBuffer, IM4_TYPE);
            break;
        default:
            printf("Data Record Read Ref: %ld\n", pCbfPrm->RecReadConf.ReqRef);
        break;
    }

    setEvent(g_setIMValuesEvent);
}

void callback_for_diagnostics_request( PNIO_CBE_PRM * pCbfPrm)
{
	if( pCbfPrm->CtrlDiagConf.ErrorCode != PNIO_OK )
	{
		printf("\nDiagnose Error! ErrorCode = %s\n", getPnioError(pCbfPrm->CtrlDiagConf.ErrorCode));

        setEvent(g_diagReqWaitEvent);
		return;
	}
    
    switch(pCbfPrm->CtrlDiagConf.pDiagData->DiagService)
	{
	    case PNIO_CTRL_DIAG_CONFIG_SUBMODULE_LIST:

            diagnosisHelper.extractDevices(pCbfPrm->CtrlDiagConf.pDiagDataBuffer, pCbfPrm->CtrlDiagConf.DiagDataBufferLen);
			break;

	    case PNIO_CTRL_DIAG_DEVICE_DIAGNOSTIC:

			diagnosisHelper.extractDiagnosisInfo(pCbfPrm->CtrlDiagConf.pDiagDataBuffer);		
			break;

	    case PNIO_CTRL_DIAG_CONFIG_NAME_ADDR_INFO:
            
			diagnosisHelper.extractAddressInfo(pCbfPrm->CtrlDiagConf.pDiagDataBuffer);
			break;

	    default:
		{
			printf("Diagnostic Request Ref: %d\n", pCbfPrm->CtrlDiagConf.pDiagData->ReqRef);
			break;
		}					
	}

    setEvent(g_diagReqWaitEvent);
}

void fillBufferWithBlankChar( PNIO_UINT8 *buffer, PNIO_UINT8 length )
{
    PNIO_BOOL bStartFilling = PNIO_FALSE;
    for(PNIO_UINT8 idx = 0; idx<length; idx++)
    {
        if(buffer[idx] == '\0')
        {
            bStartFilling = PNIO_TRUE;
        }

        if(bStartFilling)
        {
            buffer[idx] = 0x20; // Blank character
        }
    }
}

PNIO_UINT32 setIMValues(PNIO_ADDR addr)
{
    char c_tmp[60];
    int i = 0;

    PNIO_UINT32 length = 60;
    PNIO_REF    reqRef = 1;
    PNIO_IM1_TYPE im1Data;
    PNIO_IM2_TYPE im2Data;
    PNIO_IM3_TYPE im3Data;
    PNIO_IM4_TYPE im4Data;
    IM_INDEX_TYPE recordIndex;
    PNIO_UINT32 result;
    
    addr.AddrType   = PNIO_ADDR_LOG;
    addr.IODataType = PNIO_IO_IN;
        
    // Create Event
    allocateEvent(g_setIMValuesEvent);
    resetEvent(g_setIMValuesEvent);

    recordIndex = IM1_TYPE;

    im1Data.blockHeader.BlockLength      = 0x3800;
    im1Data.blockHeader.BlockType        = 0x2100;
    im1Data.blockHeader.BlockVersionHigh = 0x01;
    im1Data.blockHeader.BlockVersionLow  = 0x00;

    //Function tag
    printf("\n\tPlease enter function tag (max 32 characters): ");

    getStringFromStdio(c_tmp,33);

    for(i=0; i<32; i++)
    {
        im1Data.imTagFunction[i] = c_tmp[i];
    }

    fillBufferWithBlankChar(im1Data.imTagFunction, sizeof(im1Data.imTagFunction));

    //Location tag
    printf("\n\tPlease enter the location tag (max 22 characters): ");
                
    getStringFromStdio(c_tmp,23);

    for(i=0; i<22; i++)
    {
        im1Data.imTagLocation[i] = c_tmp[i];
    }

    fillBufferWithBlankChar(im1Data.imTagLocation, sizeof(im1Data.imTagLocation));

    result = PNIO_rec_write_req(g_ApplHandle, &addr, recordIndex, reqRef, length, (PNIO_UINT8*) &im1Data);
    if(result != PNIO_OK)
    {
        return result;
    }

    // wait event
    waitEvent(g_setIMValuesEvent);
    resetEvent(g_setIMValuesEvent);

    result = diagnosisHelper.getErrorCode();
    if(result != PNIO_OK)
    {
        return result;
    }

    recordIndex = IM2_TYPE;

    im2Data.blockHeader.BlockLength      = 0x1200;
    im2Data.blockHeader.BlockType        = 0x2200;
    im2Data.blockHeader.BlockVersionHigh = 0x01;
    im2Data.blockHeader.BlockVersionLow  = 0x00;

    //Datum input
    printf("\n\tPlease enter build in date: (YEAR-MONTH-DAY: 2014-12-31): ");
          
    getStringFromStdio(c_tmp,17);

    for(i=0; i<16; i++)
    {
        im2Data.imDate[i] = c_tmp[i];
    }

    fillBufferWithBlankChar(im2Data.imDate, sizeof(im2Data.imDate));

    result = PNIO_rec_write_req(g_ApplHandle, &addr, recordIndex, reqRef, 22, (PNIO_UINT8*) &im2Data);
    if(result != PNIO_OK)
    {
        return result;
    }

    // wait event
    waitEvent(g_setIMValuesEvent);
    resetEvent(g_setIMValuesEvent);

    result = diagnosisHelper.getErrorCode();
    if(result != PNIO_OK)
    {
        return result;
    }

    recordIndex = IM3_TYPE;

    im3Data.blockHeader.BlockLength      = 0x3800;
    im3Data.blockHeader.BlockType        = 0x2300;
    im3Data.blockHeader.BlockVersionHigh = 0x01;
    im3Data.blockHeader.BlockVersionLow  = 0x00;

    //Descriptor input
    printf("\n\tPlease enter descriptor (max 54 characters): ");
                
    getStringFromStdio(c_tmp,55);

    for(i=0; i<54;i++)
    {
        im3Data.imDescriptor[i] = c_tmp[i];
    }

    fillBufferWithBlankChar(im3Data.imDescriptor, sizeof(im3Data.imDescriptor));

    result = PNIO_rec_write_req(g_ApplHandle, &addr, recordIndex, reqRef, length, (PNIO_UINT8*) &im3Data);
    if(result != PNIO_OK)
    {
        return result;
    }

    // wait event
    waitEvent(g_setIMValuesEvent);
    resetEvent(g_setIMValuesEvent);

    result = diagnosisHelper.getErrorCode();
    if(result != PNIO_OK)
    {
        return result;
    }

    recordIndex = IM4_TYPE;

    im4Data.blockHeader.BlockLength      = 0x3800;
    im4Data.blockHeader.BlockType        = 0x2300;
    im4Data.blockHeader.BlockVersionHigh = 0x01;
    im4Data.blockHeader.BlockVersionLow  = 0x00;

    // Signature input
    printf("\n\tPlease enter new signature (max 54): ");
                
    getStringFromStdio(c_tmp,55);

    for(i=0; i<54;i++)
    {
        im4Data.imSignature[i] = c_tmp[i];
    }

    fillBufferWithBlankChar(im4Data.imSignature, sizeof(im4Data.imSignature));

    result = PNIO_rec_write_req(g_ApplHandle, &addr, recordIndex, reqRef, length, (PNIO_UINT8*) &im4Data);
    if(result != PNIO_OK)
    {
        return result;
    }

    // wait event
    waitEvent(g_setIMValuesEvent);

    result = diagnosisHelper.getErrorCode();
    if(result != PNIO_OK)
    {
        return result;
    }

    //free event
    freeEvent(g_setIMValuesEvent);

    return result;
}

int main()
{
    int  cmd = 777;
    PNIO_UINT32 result;
    PNIO_ADDR laddr;

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
        printf("PROFINET Driver | Diagnose Request user application \n");
        printf("=================================================== \n");
        printf("1...Startup                                         \n");
        printf("2...Show submodule list                             \n");
        printf("3...Show device diagnostic                          \n");
        printf("4...Show device configuration and diagnostics       \n");
        printf("5...Show address info                               \n");
        printf("6...Set device I&M values                           \n");
        printf("0...Exit                                            \n\n");
        
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
                startUpPnd("PNDriverBase_PNDiagose/Station_1.PN Driver_1.PNDriverConfiguration.xml", "rema.xml");

                if( (result = PNIO_interface_open(1, callback_for_ifc_record_read_conf, callback_for_alarm_ind, &g_ApplHandle)) != PNIO_OK)
                {
                    printf("PNIO_interface_open() returned error: %s\n", getPnioError(result));
                }
                else if((result = PNIO_controller_open(1, PNIO_CEP_MODE_CTRL, callback_for_ds_read_conf, callback_for_ds_write_conf, callback_for_alarm_ind, &g_ApplHandle)) != PNIO_OK)
                {
                    printf("PNIO_controller_open() returned error: %s\n", getPnioError(result));
                }
                else if((result = PNIO_register_cbf( g_ApplHandle, PNIO_CBE_CTRL_DIAG_CONF, callback_for_diagnostics_request)) != PNIO_OK)
                {
                    printf("PNIO_register_cbf() returned error: %s\n", getPnioError(result));
                }
                else if((result = PNIO_set_mode(g_ApplHandle,PNIO_MODE_OPERATE)) != PNIO_OK)
                {
                    printf("PNIO_register_cbf() returned error: %s\n", getPnioError(result));
                }
                g_diagReqWaitEvent = 0;
                g_setIMValuesEvent = 1;
                break;

            case 2: // Show submodule list
                result = diagnosisHelper.makeDiagnosisRequest(g_ApplHandle, PNIO_CTRL_DIAG_CONFIG_SUBMODULE_LIST, g_diagReqWaitEvent);

                if(result == PNIO_OK)
                {
                    diagnosisHelper.printSubmoduleList();
                }
                else
                {
                    printf("PNIO_ctrl_diag_req() returned error: %s\n", getPnioError(result));
                }
                break;

            case 3: // Show device diagnostic
                printf("Logical address: ");

                scanf("%d", (PNIO_UINT32*)&laddr.u.Addr);

                result = diagnosisHelper.makeDiagnosisRequest(g_ApplHandle, PNIO_CTRL_DIAG_DEVICE_DIAGNOSTIC, g_diagReqWaitEvent, laddr.u.Addr);

                if(result == PNIO_OK)
                {
                    diagnosisHelper.printDiagnosisInfo();
                }
                else
                {
                    printf("PNIO_ctrl_diag_req() returned error: %s\n", getPnioError(result));
                }
                break;

            case 4: // Show device configuration and diagnostics
                result = diagnosisHelper.getDeviceConfAndDiag(g_ApplHandle, g_diagReqWaitEvent, g_setIMValuesEvent);

                if(result == PNIO_OK)
                {
                    //print
                    diagnosisHelper.printDevice();
                }
                else
                {
                    printf("Device config and diagnostic returned error: %s\n", getPnioError(result));
                }
                break; 
            case 5: // Show address info
                result = diagnosisHelper.makeDiagnosisRequest(g_ApplHandle, PNIO_CTRL_DIAG_CONFIG_NAME_ADDR_INFO, g_diagReqWaitEvent);
    
                if(result == PNIO_OK)
                {
                    diagnosisHelper.printAddressInfo();
                }
                else
                {
                    printf("PNIO_ctrl_diag_req() returned error: %s\n", getPnioError(result));
                }

                break;

            case 6: // Set IM Values
                printf("Logical address: ");

                scanf("%d", (PNIO_UINT32*)&laddr.u.Addr);

                result = setIMValues(laddr);

                if(result != PNIO_OK)
                {
                    printf("PNIO_rec_write_req() returned error: %s\n", getPnioError(result));
                }
                else
                {
                    printf("Device IM Values have been successfully set!\n");
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