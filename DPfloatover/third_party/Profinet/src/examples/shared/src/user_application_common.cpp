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
/*  F i l e               &F: user_application_common.cpp               :F&  */
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
#include <psi_cfg.h>
#include "stdio.h"

#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
#include "conio.h"
#endif

#include <string>
#include <map>

#include "pniobase.h"
#include "servusrx.h"
#include "pniousrx.h"

#include "eps_sys.h"

#include "pnd_int.h"
#include "pnd_sys.h"

#include "helper_functions.h"
#include "user_application_common.h"

#define PND_INVALID_HANDLE  0xFFFF
#define PND_MAX_NR_OF_CP 50

extern "C" 
{
    PNIO_UINT32 g_ApplHandle = PND_INVALID_HANDLE;
}

FILE* pTraceFile = 0;
PNIO_DEBUG_SETTINGS_TYPE debSet;
std::map<int, std::string> pnioErrorMap;

void updateTraceFile
       ( PNIO_UINT8       * pBuffer,
         PNIO_UINT32        BufferSize )
{
    if (pTraceFile != 0) 
    {
        fwrite(pBuffer,1,BufferSize,pTraceFile);
        fflush(pTraceFile);
    }
}

void initPnd()
{        
    //open trace buffer file
    pTraceFile = fopen("pnd_trace.bin","wb");

    memset( &debSet, 0, sizeof ( PNIO_DEBUG_SETTINGS_TYPE ));
                     
    debSet.CbfPntrcBufferFull = updateTraceFile;

    debSet.TraceLevels[PNIO_TRACE_COMP_ACP]      = PNIO_TRACE_LEVEL_WARN;
    debSet.TraceLevels[PNIO_TRACE_COMP_ADONIS]   = PNIO_TRACE_LEVEL_WARN;
    debSet.TraceLevels[PNIO_TRACE_COMP_CLRPC]    = PNIO_TRACE_LEVEL_WARN;
    debSet.TraceLevels[PNIO_TRACE_COMP_CM]       = PNIO_TRACE_LEVEL_WARN;
    debSet.TraceLevels[PNIO_TRACE_COMP_DCP]      = PNIO_TRACE_LEVEL_WARN;
    debSet.TraceLevels[PNIO_TRACE_COMP_EDDI]     = PNIO_TRACE_LEVEL_WARN;
    debSet.TraceLevels[PNIO_TRACE_COMP_EDDP]     = PNIO_TRACE_LEVEL_WARN;
    debSet.TraceLevels[PNIO_TRACE_COMP_EDDS]     = PNIO_TRACE_LEVEL_WARN;
    debSet.TraceLevels[PNIO_TRACE_COMP_EPS]      = PNIO_TRACE_LEVEL_WARN;
    debSet.TraceLevels[PNIO_TRACE_COMP_GSY]      = PNIO_TRACE_LEVEL_WARN;
    debSet.TraceLevels[PNIO_TRACE_COMP_HIF]      = PNIO_TRACE_LEVEL_WARN;
    debSet.TraceLevels[PNIO_TRACE_COMP_IOH]      = PNIO_TRACE_LEVEL_WARN;
    debSet.TraceLevels[PNIO_TRACE_COMP_LLDP]     = PNIO_TRACE_LEVEL_WARN;
    debSet.TraceLevels[PNIO_TRACE_COMP_MRP]      = PNIO_TRACE_LEVEL_WARN;
    debSet.TraceLevels[PNIO_TRACE_COMP_NARE]     = PNIO_TRACE_LEVEL_WARN;
    debSet.TraceLevels[PNIO_TRACE_COMP_OHA]      = PNIO_TRACE_LEVEL_WARN;
    debSet.TraceLevels[PNIO_TRACE_COMP_PNTRC]    = PNIO_TRACE_LEVEL_WARN;
    debSet.TraceLevels[PNIO_TRACE_COMP_POF]      = PNIO_TRACE_LEVEL_WARN;
    debSet.TraceLevels[PNIO_TRACE_COMP_PSI]      = PNIO_TRACE_LEVEL_WARN;
    debSet.TraceLevels[PNIO_TRACE_COMP_SNMPX]    = PNIO_TRACE_LEVEL_WARN;
    debSet.TraceLevels[PNIO_TRACE_COMP_SOCK]     = PNIO_TRACE_LEVEL_WARN;
    debSet.TraceLevels[PNIO_TRACE_COMP_TCIP]     = PNIO_TRACE_LEVEL_WARN;
    debSet.TraceLevels[PNIO_TRACE_COMP_PCIOX]    = PNIO_TRACE_LEVEL_WARN;
    debSet.TraceLevels[PNIO_TRACE_COMP_PNTEST]   = PNIO_TRACE_LEVEL_WARN;
    debSet.TraceLevels[PNIO_TRACE_COMP_PND]      = PNIO_TRACE_LEVEL_WARN;        
}

void startUpPnd(char* pHwXmlName, char* pRemaName)
{
    PNIO_UINT32 result = 0;
    PNIO_UINT32 hwFileSize = 0;
    PNIO_UINT32 remaFileSize = 0;
    PNIO_CP_ID_TYPE locID;

    FILE* hwFile = NULL;
	FILE* remaFile = NULL;
    char* hwFileBuffer = NULL;
    char* remaFileBuffer = NULL;	

    if(pRemaName != NULL)
    {
	    remaFile = fopen(pRemaName, "rb");  

        if(remaFile == NULL)
        {
            printf("Rema file %s could not be opened\n", pRemaName);
        }
        else
        {
            fseek(remaFile, 0, SEEK_END);
            remaFileSize = ftell(remaFile);
            fseek(remaFile, 0, SEEK_SET);
            remaFileBuffer = (char*)malloc(remaFileSize+1);
            fread(remaFileBuffer, 1, remaFileSize, (FILE*) remaFile);
            remaFileBuffer[remaFileSize] = 0; //add terminating NULL
            fclose(remaFile);            
        }
    }

    hwFile = fopen(pHwXmlName, "rb");
    if( hwFile == NULL )
    {
        printf( "Error: Hardware configuration  file '%s' could not be opened!\n",  pHwXmlName);
    }
    else
    {
        fseek(hwFile, 0, SEEK_END);
        hwFileSize = ftell(hwFile);
        fseek(hwFile, 0, SEEK_SET);
        hwFileBuffer = (char*)malloc(hwFileSize+1);
        fread(hwFileBuffer, 1, hwFileSize, (FILE*) hwFile);
        hwFileBuffer[hwFileSize] = 0; //add terminating NULL
        fclose(hwFile);
        
        result = SERV_CP_init(&debSet);

        if(result == PNIO_OK)
        {
            PNIO_CP_ID_PTR_TYPE pCpList;
            PNIO_UINT8 nrOfcp;

            pCpList = (PNIO_CP_ID_PTR_TYPE) malloc( sizeof ( PNIO_CP_ID_TYPE ) * PND_MAX_NR_OF_CP );

            memset( &locID, 0, sizeof ( PNIO_CP_ID_TYPE ));
            memset( pCpList, 0, sizeof ( PNIO_CP_ID_TYPE ) * PND_MAX_NR_OF_CP );

            result = SERV_CP_get_network_adapters(pCpList, &nrOfcp);

            if (result == PNIO_OK)
            {
                getNetworkAdapterSelection(&locID, pCpList, nrOfcp);

                PNIO_SYSTEM_DESCR sd;
                strcpy((char*)(&sd.Vendor), "Siemens AG");
                strcpy((char*)(&sd.ProductFamily), "PN DRIVER");        
                strcpy((char*)(&sd.IM_DeviceType), "PN DRIVER");
                strcpy((char*)(&sd.IM_OrderId), "6ES7195-3AA00-0YA0");
                sd.IM_HwRevision = 0;
                sd.IM_SwVersion.revision_prefix = 'V';
                sd.IM_SwVersion.functional_enhancement = 1;
                sd.IM_SwVersion.bug_fix = 1;                            
                sd.IM_SwVersion.internal_change = 0;                            
                strcpy((char*)(&sd.ProductSerialNr), "0000000");

                if(remaFile != NULL)
                {
                    printf("Startup will be executed with rema ...\n");
                }
            
 			    result = SERV_CP_startup( &locID, 1, (PNIO_UINT8*)hwFileBuffer, hwFileSize, (PNIO_UINT8*)remaFileBuffer, remaFileSize, &sd);
			    if (result == PNIO_ERR_INVALID_REMA)
			    {
				    printf ("Invalid Rema file!\n");
			    }
                printf( "SERV_CP_startup() Result: %s\n", getPnioError(result) );
            }
            free(pCpList);
        }
        else
        {
            printf( "SERV_CP_init() Result: %s\n", getPnioError(result) );
        }

        if(remaFile != NULL)
        {
            free(remaFileBuffer);
        }
           
        free(hwFileBuffer);
    }
}

const char* getPnioError(int error)
{
    static std::string str;

    if(error < 0)
    {
        return NULL;
    }

    if(pnioErrorMap.empty())
    {
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_PRM_HND);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_PRM_BUF);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_PRM_LEN);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_PRM_ADD);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_PRM_RSTATE);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_PRM_CALLBACK);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_PRM_TYPE);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_PRM_EXT_PAR);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_PRM_IO_TYPE);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_PRM_CP_ID);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_PRM_LOC_STATE);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_PRM_REC_INDEX);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_PRM_TIMEOUT);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_PRM_DEV_ANNOTATION);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_PRM_DEV_STATE);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_PRM_PCBF);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_PRM_MAX_AR_VALUE);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_PRM_ACCESS_TYPE);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_PRM_POINTER);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_PRM_INVALIDARG);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_PRM_MEASURE_NUMBER);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_PRM_CYCLE_OFFSET);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_PRM_ROUTER_ADD);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_PRM_IP);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_PRM_NOS);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_PRM_NOS_LEN);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_WRONG_HND);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_MAX_REACHED);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_CREATE_INSTANCE);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_MODE_VALUE);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_OPFAULT_NOT_REG);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_NEWCYCLE_SEQUENCE_REG);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_NETWORK_PROT_NOT_AVAILABLE);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_NO_CONNECTION);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_OS_RES);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_ALREADY_DONE);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_ALLREADY_DONE);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_NO_CONFIG);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_SET_MODE_NOT_ALLOWED);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_DEV_ACT_NOT_ALLOWED);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_NO_LIC_SERVER);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_VALUE_LEN);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_SEQUENCE);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_INVALID_CONFIG);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_UNKNOWN_ADDR);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_NO_RESOURCE);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_CONFIG_IN_UPDATE);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_NO_FW_COMMUNICATION);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_STARTOP_NOT_REGISTERED);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_OWNED);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_START_THREAD_FAILED);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_START_RT_THREAD_FAILED);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_DRIVER_IOCTL_FAILED);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_AFTER_EXCEPTION);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_NO_CYCLE_INFO_DATA);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_SESSION);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_ALARM_DATA_FORMAT);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_ABORT);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_CORRUPTED_DATA);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_FLASH_ACCESS);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_WRONG_RQB_LEN);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_NO_RESET_VERIFICATION);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_SET_IP_NOS_NOT_ALLOWED);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_INVALID_REMA);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_NOT_REENTERABLE);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_INVALID_STATION);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_INVALID_PORT);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_NO_ADAPTER_FOUND);
        INSERT_PNIO_ERROR_TO_MAP(PNIO_ERR_INTERNAL);
        }

    if(pnioErrorMap.count(error) == 1)
    {
        str = pnioErrorMap[error];
    }
    else
    {
        char num[10];
        sprintf(num, "%d", error);
        str = std::string(num);
    }

    return str.c_str();
}
/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
