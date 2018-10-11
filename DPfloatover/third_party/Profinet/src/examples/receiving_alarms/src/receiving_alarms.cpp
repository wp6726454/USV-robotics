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
/*  F i l e               &F: receiving_alarms.cpp                      :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*    This example application shows how to receive alarms from the local    */
/*  IOInterface or from any other remote IODevice. In order to receive       */ 
/*  alarms from these interface, callback function is registered to that     */
/*  particular interface.                                                    */
/*                                                                           */
/*    The callback function in the example handles all alarms by displaying  */
/*  the alarm information on the console and stores the alarm information    */
/*  as an alarm item in a ring buffer. Init alarm buffer clears the ring     */
/*  buffer.                                                                  */
/*                                                                           */
/*    Show alarm buffer displays received alarms in the ring buffer.         */
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
#include "helper_functions.h"
#include "user_application_common.h"
#include <string>
#include <map>
#include "alarm_store.h"

#define MAX_NUMBER_OF_ALARM_ITEMS   128

extern "C" 
{
    extern PNIO_UINT32 g_ApplHandle;
}
extern FILE* pTraceFile;

#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
    HANDLE m_bufferMutex;
#elif (EPS_PLF == EPS_PLF_LINUX_X86)
    pthread_mutex_t m_bufferMutex;
#endif

CAlarmStore g_AlarmStore;
void callback_for_ifc_record_read_conf(PNIO_CBE_PRM*) {}
void callback_for_ds_read_conf(PNIO_CBE_PRM*) {}
void callback_for_ds_write_conf(PNIO_CBE_PRM*) {}
void printAlarm(const AlarmItem_s* alarmItem);

void initMutex()
{
#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
    m_bufferMutex = CreateMutex( NULL, FALSE, NULL);
#elif (EPS_PLF == EPS_PLF_LINUX_X86)
    pthread_mutex_init(&m_bufferMutex, NULL);
#endif    
}

void lockMutex()
{
#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
    WaitForSingleObject(m_bufferMutex, INFINITE);
#elif (EPS_PLF == EPS_PLF_LINUX_X86)
    pthread_mutex_lock(&m_bufferMutex);
#endif
}

void unlockMutex()
{
#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
    ReleaseMutex(m_bufferMutex);
#elif (EPS_PLF == EPS_PLF_LINUX_X86)
    pthread_mutex_unlock(&m_bufferMutex);
#endif
}

void callback_for_alarm_ind(PNIO_CBE_PRM* pCbfPrm)
{
    #if (EPS_PLF == EPS_PLF_WINDOWS_X86)
    waitForConsole();
    #endif      

    if(pCbfPrm->CbeType == PNIO_CBE_ALARM_IND || pCbfPrm->CbeType == PNIO_CBE_IFC_ALARM_IND)
    {
        lockMutex();
        g_AlarmStore.addNewAlarm(pCbfPrm->AlarmInd);
        printAlarm( g_AlarmStore.getAlarmByIndex(g_AlarmStore.getLastAlarmIndex()) );
        unlockMutex();
    }
}

std::string getDiagnosisAlarmText(unsigned int channelErrorType, unsigned int extChannelErrorType, bool extended)
{
    static bool mapInitialized = false;    
    static std::map<unsigned int, std::string> channelMap;
    static std::map<unsigned int, std::map<unsigned int, std::string> > extChannelMap;
    std::string returnString = "Unknown or Manufacturer specific error";

    if(mapInitialized == false)
    {
        mapInitialized = true;
        channelMap[0x0001] = "Short circuit";
        channelMap[0x0002] = "Undervoltage";
        channelMap[0x0003] = "Overvoltage";
        channelMap[0x0004] = "Overload";
        channelMap[0x0005] = "Over temperature";
        channelMap[0x0006] = "Line break";
        channelMap[0x0007] = "Upper limit value exceeded";
        channelMap[0x0008] = "Lower limit value exceeded";
        channelMap[0x0009] = "Error";
        channelMap[0x000A] = "Simulation active";
        channelMap[0x000F] = "The channel needs (additional) parameters. No or too less parameters are written";
        channelMap[0x0010] = "Parameterization fault. Wrong or too many parameters are written";
        channelMap[0x0011] = "Power supply fault";
        channelMap[0x0012] = "Fuse blown / open";
        channelMap[0x0013] = "Communication fault. Sequence number wrong / sequence wrong";
        channelMap[0x0014] = "Ground fault";
        channelMap[0x0015] = "Reference point lost";
        channelMap[0x0016] = "Process event lost / sampling error";
        channelMap[0x0017] = "Threshold warning";
        channelMap[0x0018] = "Output disabled";
        channelMap[0x0019] = "FunctionalSafety event";
        channelMap[0x001A] = "External fault";
        channelMap[0x001F] = "Temporary fault";
        channelMap[0x0040] = "Mismatch of safety destination address";
        channelMap[0x0041] = "Safety destination address not valid";
        channelMap[0x0042] = "Safety source address not valid or mismatch";
        channelMap[0x0043] = "Safety watchdog time value is 0 ms";
        channelMap[0x0044] = "Parameter \"F_SIL\" exceeds SIL from specific device application";
        channelMap[0x0045] = "Parameter \"F_CRC_Length\" does not match the generated values";
        channelMap[0x0046] = "Version of F-Parameter set incorrect";
        channelMap[0x0047] = "Data inconsistent in received F-Parameter block (CRC1 error)";
        channelMap[0x0048] = "Device specific or unspecified diagnosis information, see manual";
        channelMap[0x0049] = "Save iParameter watchdog time exceeded";
        channelMap[0x004A] = "Restore iParameter watchdog time exceeded";
        channelMap[0x004B] = "Inconsistent iParameters (iParCRC error)";
        channelMap[0x004C] = "F_Block_ID not supported";
        channelMap[0x004D] = "Transmission error: data inconsistent (CRC2 error)";
        channelMap[0x004E] = "Transmission error: timeout (F_WD_Time or F_WD_Time_2 elapsed)";
        channelMap[0x8000] = "Data Transmission Impossible";
        channelMap[0x8001] = "Remote Mismatch";
        channelMap[0x8002] = "Media Redundancy Mismatch";
        channelMap[0x8003] = "Sync Mismatch";
        channelMap[0x8004] = "Isochronous Mode Mismatch";
        channelMap[0x8005] = "Multicast CR Mismatch";
        channelMap[0x8007] = "Information for fiber optic links";
        channelMap[0x8008] = "Network functionality problems occur";
        channelMap[0x8009] = "Time master not existent or precision problems";
        channelMap[0x800A] = "DFP problems occur";
        channelMap[0x800B] = "MRPD problems occur";
        channelMap[0x800D] = "Information about multiple interface problems";
        channelMap[0x800E] = "Used for propagation of diagnosis in hierarchical structures";

        extChannelMap[0x8000][0x8000] = "Link State mismatch - Link down";
        extChannelMap[0x8000][0x8001] = "MAUType mismatch";
        extChannelMap[0x8000][0x8002] = "Line Delay mismatch";
        extChannelMap[0x8001][0x8000] = "Peer name of station mismatch";
        extChannelMap[0x8001][0x8001] = "Peer name of port mismatch";
        extChannelMap[0x8001][0x8002] = "Peer RT_CLASS_3 mismatch";
        extChannelMap[0x8001][0x8003] = "Peer MAUType mismatch";
        extChannelMap[0x8001][0x8004] = "Peer MRP domain mismatch";
        extChannelMap[0x8001][0x8005] = "No peer detected";
        extChannelMap[0x8001][0x8007] = "Peer Line Delay mismatch";
        extChannelMap[0x8001][0x8008] = "Peer PTCP mismatch";
        extChannelMap[0x8001][0x8009] = "Peer Preamble Length mismatch";
        extChannelMap[0x8001][0x800A] = "Peer Fragmentation mismatch";
        extChannelMap[0x8002][0x8000] = "Manager role fail";
        extChannelMap[0x8002][0x8001] = "MRP ring open";
        extChannelMap[0x8002][0x8003] = "Multiple manager";
        extChannelMap[0x8002][0x8004] = "Dedicated partner ring port missing";
        extChannelMap[0x8003][0x8000] = extChannelMap[0x8009][0x8000] = "No sync message received";
        extChannelMap[0x8003][0x8003] = extChannelMap[0x8009][0x8003] = "Jitter out of boundary";
        extChannelMap[0x8004][0x8000] = "Output Time Failure - Output update missing or out of order";
        extChannelMap[0x8004][0x8001] = "Input Time Failure";
        extChannelMap[0x8004][0x8002] = "Master Life Sign Failure - Error in MLS update detected";
        extChannelMap[0x8005][0x8000] = "Multicast Consumer CR timed out";
        extChannelMap[0x8005][0x8001] = "Address resolution failed";
        extChannelMap[0x8007][0x8000] = "Power Budget";
        extChannelMap[0x8008][0x8000] = "Frame dropped - no resource";
        extChannelMap[0x800A][0x8100] = "Frame late error for FrameID \"0x0100\"";
        extChannelMap[0x800A][0x8FFF] = "Frame late error for FrameID \"0x0FFF\"";
        extChannelMap[0x800B][0x8700] = "MRPD duplication void for FrameID \"0x0700\"";
        extChannelMap[0x800B][0x8FFF] = "MRPD duplication void for FrameID \"0x0FFF\"";
        extChannelMap[0x800D][0x8000] = "Standard Gateway mismatch";
        extChannelMap[0x800D][0x8001] = "NameOfStation of the interface is not unambiguous";
        extChannelMap[0x800D][0x8002] = "IP Address range of the interface is not unambiguous";
        extChannelMap[0x800D][0x8003] = "Conflicting MultipleInterfaceMode.NameOfDevice mode";
        extChannelMap[0x800E][0x8000] = "Subordinated entity information";
    }

    if(channelMap.count(channelErrorType) == 1)
    {
        returnString = channelMap[channelErrorType];

        std::string extChannelString = "";
        if(extended == true)
        {
            if( (channelErrorType >= 0x00 && channelErrorType <= 0xFF) || (channelErrorType >= 0x0100 && channelErrorType <= 0x7FFF) )
            {
                if(extChannelErrorType == 0x8000)
                {
                    extChannelString = "Accumulative Info";
                }
            }
            else if(extChannelMap.count(channelErrorType) == 1)
            {
                if(extChannelMap[channelErrorType].count(extChannelErrorType) == 1)
                {
                    extChannelString = extChannelMap[channelErrorType][extChannelErrorType];
                }
            }
        }

        if(extChannelString != "")
        {
            returnString += ": " + extChannelString;
        }
    }

    return returnString;
}

std::string getAlarmText(PNIO_ALARM_TYPE alarmType)
{
    std::string returnString;

    switch(alarmType)
    {
    case PNIO_ALARM_DIAGNOSTIC:
        returnString = "Alarm diagnostic";
        break;
    case PNIO_ALARM_PROCESS:
        returnString = "Alarm process";
        break;
    case PNIO_ALARM_PULL:
        returnString = "Alarm pull";
        break;
    case PNIO_ALARM_PLUG:
        returnString = "Alarm plug";
        break;
    case PNIO_ALARM_STATUS:
        returnString = "Alarm status";
        break;
    case PNIO_ALARM_UPDATE:
        returnString = "Alarm update";
        break;
    case PNIO_ALARM_REDUNDANCY:
        returnString = "Alarm redundancy";
        break;
    case PNIO_ALARM_CONTROLLED_BY_SUPERVISOR:
        returnString = "Alarm controlled by supervisor";
        break;
    case PNIO_ALARM_RELEASED_BY_SUPERVISOR:
        returnString = "Alarm released by supervisor";
        break;
    case PNIO_ALARM_PLUG_WRONG:
        returnString = "Alarm plug wrong";
        break;
    case PNIO_ALARM_RETURN_OF_SUBMODULE:
        returnString = "Alarm return of submodule";
        break;
    case PNIO_ALARM_DIAGNOSTIC_DISAPPEARS:
        returnString = "Alarm diagnostic disappears";
        break;
    case PNIO_ALARM_MCR_MISMATCH:
        returnString = "Alarm mcr mismatch";
        break;
    case PNIO_ALARM_PORT_DATA_CHANGED:
        returnString = "Alarm port data changed";
        break;
    case PNIO_ALARM_SYNC_DATA_CHANGED:
        returnString = "Alarm sync data changed";
        break;
    case PNIO_ALARM_ISOCHRONE_MODE_PROBLEM:
        returnString = "Alarm isochrone mode problem";
        break;
    case PNIO_ALARM_NETWORK_COMPONENT_PROBLEM:
        returnString = "Alarm network component problem";
        break;
    case PNIO_ALARM_TIME_DATA_CHANGED:
        returnString = "Alarm time data changed";
        break;
    case PNIO_ALARM_UPLOAD_AND_STORAGE:
        returnString = "Alarm upload and storage";
        break;
    case PNIO_ALARM_PULL_MODULE:
        returnString = "Alarm pull module";
        break;
    case PNIO_ALARM_DEV_FAILURE:
        returnString = "Alarm dev failure";
        break;
    case PNIO_ALARM_DEV_RETURN:
        returnString = "Alarm dev return";
        break;
    default:
        break;
    }

    return returnString;
}

void printAlarm(const AlarmItem_s* alarmItem)
{
    printf("********************************************************************************************\n");
    printf("Id: %u\n", alarmItem->id);
    printf("LogAddr %u | DeviceNr: %u | SlotNr: %u | SubslotNr: %u | ModulIdent: %u | SubmodulIdent: %u\n", 
        alarmItem->address.u.Addr, alarmItem->alarmData.DeviceNum, alarmItem->alarmData.SlotNum, alarmItem->alarmData.SubslotNum,
        alarmItem->alarmData.AlarmAinfo.ModIdent, alarmItem->alarmData.AlarmAinfo.SubIdent);
    
    std::string alarmText;

    switch(alarmItem->alarmData.AlarmAinfo.UserStrucIdent)
    {
    case 0x8000:
        alarmText = getDiagnosisAlarmText(alarmItem->alarmData.AlarmAinfo.UAData.diag.ChannelErrorType, 0, false);
        break;
    case 0x8002:
        alarmText = getDiagnosisAlarmText(alarmItem->alarmData.AlarmAinfo.UAData.ext_diag.ChannelErrorType, 
                                    alarmItem->alarmData.AlarmAinfo.UAData.ext_diag.ExtChannelErrorType, true);
        break;
    case 0x8100:
        alarmText = getDiagnosisAlarmText(alarmItem->alarmData.AlarmAinfo.UAData.m_diag.ChannelErrorType, 0, false);
        break;
    default:
        alarmText = getAlarmText(alarmItem->alarmData.AlarmType);
        break;
    }

    if(alarmText != "")
    {
        printf("%s\n", alarmText.c_str());
    }
}

void showAlarmBuffer()
{
    lockMutex();

    bool empty = g_AlarmStore.isEmpty();    

    if(empty == true)
    {
        printf("Alarm buffer is empty\n");
    }
    else
    {
        unsigned int alarmToRead = g_AlarmStore.getFirstAlarmIndex();
        unsigned int lastAlarm = g_AlarmStore.getLastAlarmIndex();        

        while(true)
        {
            printAlarm( g_AlarmStore.getAlarmByIndex(alarmToRead) );
            if(alarmToRead == lastAlarm)
            {
                break;
            }
            else
            {
                alarmToRead = g_AlarmStore.calculateNextIndex(alarmToRead);
            }
        }
    }

    unlockMutex();
}

void initBuffer()
{
    lockMutex();
    g_AlarmStore.init();
    unlockMutex();
}

int main()
{
    int  cmd = 777;
    PNIO_UINT32 result;
    #if (EPS_PLF == EPS_PLF_WINDOWS_X86)
    _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
    initConsoleLock();
    #endif

    initMutex();
    initPnd();

    startUpPnd("PNDriverBase_ReceivingAlarms/Station_1.PN Driver_1.PNDriverConfiguration.xml", NULL);                

    if( (result = PNIO_interface_open(1, callback_for_ifc_record_read_conf ,callback_for_alarm_ind , &g_ApplHandle)) != PNIO_OK)
    {
        printf("PNIO_interface_open() returned error: %s\n", getPnioError(result));
    }
    else if( (result = PNIO_controller_open(1, PNIO_CEP_MODE_CTRL,callback_for_ds_read_conf ,callback_for_ds_write_conf ,callback_for_alarm_ind , &g_ApplHandle)) != PNIO_OK)
    {
        printf("PNIO_controller_open() returned error: %s\n", getPnioError(result));
    }
    else if( (result = PNIO_set_mode(g_ApplHandle,PNIO_MODE_OPERATE)) == PNIO_OK)
    {
        printf("Mode has been set to operate\n");
    }
    else
    {
        printf("PNIO_set_mode() returned error: %s\n", getPnioError(result));
    }

    while (cmd != 0)
    {
        #if (EPS_PLF == EPS_PLF_WINDOWS_X86)
        lockConsole();
        #endif
        printf("\n");
        printf("PROFINET Driver | Receiving Alarms user application\n");
        printf("================================\n");
        printf("1...Show alarm buffer           \n");
        printf("2...Init alarm buffer           \n" );
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
            showAlarmBuffer();
            break;
        case 2:
            initBuffer();
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

    return 0;
}
