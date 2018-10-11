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
/*  F i l e               &F: pnd_test.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Test Application for PNDriver                                            */
/*                                                                           */
/*****************************************************************************/
#define _CRTDBG_MAP_ALLOC
#include <psi_cfg.h>
#include <stdlib.h>

#define LTRC_ACT_MODUL_ID 0000
#define PND_MODULE_ID 0000

#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
#include <crtdbg.h>
#endif

#include <stdio.h>
#include <string.h>

#include "pnd_int.h"
#include "pnd_test.h"
#include "pniobase.h"
#include "servusrx.h"

#if (EPS_PLF == EPS_PLF_LINUX_X86)
#include <signal.h>
#endif

static FILE* PNTRC_fp = 0;
static PNIO_DEBUG_SETTINGS_TYPE debSet;
extern PNIO_UINT32 g_ApplHandle;

#define PND_MAX_NR_OF_CP 50

void HR(int iLen) {
  int i;

  for (i = 0; i < iLen; i++) printf("-");

  printf("\n");
  return;
}

void pnd_test_buffer_full(PNIO_UINT8* pBuffer, PNIO_UINT32 BufferSize) {
  if (PNTRC_fp != 0) {
    fwrite(pBuffer, 1, BufferSize, PNTRC_fp);
    fflush(PNTRC_fp);
  }
}

#if (EPS_PLF == EPS_PLF_LINUX_X86)
void terminationHandler(int sigNum) {
  pnd_test_quit_application(g_ApplHandle);
  exit(1);
}
#endif

void pnd_init() {
#if (EPS_PLF == EPS_PLF_LINUX_X86)
  struct sigaction signalStr;

  signalStr.sa_handler = terminationHandler;
  sigemptyset(&signalStr.sa_mask);
  signalStr.sa_flags = 0;
  sigaction(SIGINT, &signalStr, NULL);
  sigaction(SIGTERM, &signalStr, NULL);
  sigaction(SIGHUP, &signalStr, NULL);
#endif

  // open trace buffer file
  PNTRC_fp = fopen("pnd_trace.bin", "wb");

  memset(&debSet, 0, sizeof(PNIO_DEBUG_SETTINGS_TYPE));

  debSet.CbfPntrcBufferFull = pnd_test_buffer_full;

  debSet.TraceLevels[PNIO_TRACE_COMP_ACP] = PNIO_TRACE_LEVEL_WARN;
  debSet.TraceLevels[PNIO_TRACE_COMP_CLRPC] = PNIO_TRACE_LEVEL_WARN;
  debSet.TraceLevels[PNIO_TRACE_COMP_CM] = PNIO_TRACE_LEVEL_WARN;
  debSet.TraceLevels[PNIO_TRACE_COMP_DCP] = PNIO_TRACE_LEVEL_WARN;
  debSet.TraceLevels[PNIO_TRACE_COMP_EDDS] = PNIO_TRACE_LEVEL_WARN;
  debSet.TraceLevels[PNIO_TRACE_COMP_EPS] = PNIO_TRACE_LEVEL_WARN;
  debSet.TraceLevels[PNIO_TRACE_COMP_HIF] = PNIO_TRACE_LEVEL_WARN;
  debSet.TraceLevels[PNIO_TRACE_COMP_LLDP] = PNIO_TRACE_LEVEL_WARN;
  debSet.TraceLevels[PNIO_TRACE_COMP_NARE] = PNIO_TRACE_LEVEL_WARN;
  debSet.TraceLevels[PNIO_TRACE_COMP_OHA] = PNIO_TRACE_LEVEL_WARN;
  debSet.TraceLevels[PNIO_TRACE_COMP_PNTRC] = PNIO_TRACE_LEVEL_WARN;
  debSet.TraceLevels[PNIO_TRACE_COMP_PSI] = PNIO_TRACE_LEVEL_WARN;
  debSet.TraceLevels[PNIO_TRACE_COMP_SOCK] = PNIO_TRACE_LEVEL_WARN;
  debSet.TraceLevels[PNIO_TRACE_COMP_TCIP] = PNIO_TRACE_LEVEL_WARN;
  debSet.TraceLevels[PNIO_TRACE_COMP_PND] = PNIO_TRACE_LEVEL_WARN;
}

int main() {
  PNIO_UINT32 ret_val = 0;
  int cmd = 777;
  char* xml_buffer = 0;
  char* rema_buffer = 0;  // For Rema XML
#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
  Console_lock_init();
#endif

  // After pnd_init you can use e.g. pntrc
  pnd_init();

  while (cmd != 0) {
#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
    Console_lock();
#endif
    printf("\n");
    HR(80);
    printf("PROFINET Driver | test program  \n");
    printf("================================\n");
    printf("1...Help (show menu)            \n");
    printf("================================\n");
    printf("2...SERV_CP_Startup             \n");
    printf("4...SERV_CP_Shutdown            \n");
    printf("================================\n");
    printf("5...Open Controller             \n");
    printf("6...Close Controller            \n");
    printf("================================\n");
    printf("7...Set Mode PNIO_MODE_OFFLINE  \n");
    printf("8...Set Mode PNIO_MODE_CLEAR    \n");
    printf("9...Set Mode PNIO_MODE_OPERATE  \n");
    printf("================================\n");
    printf("10...Device Activate            \n");
    printf("11...Device Deactivate          \n");
    printf("================================\n");
    printf("12...I&M Data Read              \n");
    printf("13...I&M Data Write             \n");
    printf("================================\n");
    printf("15...SERV_CP_set_trace_level    \n");
    printf("================================\n");
    printf("16...IO Data Read/Write         \n");
    printf("=============================== \n");
    printf("0... QUIT \n");

#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
    Console_unlock();
    scanf_s("%d", &cmd);
#else
    scanf("%d", &cmd);
#endif

    while (getchar() != '\n')
      ;
    switch (cmd) {
      case 1:
        break;  // Show menu
      case 2:   // SERV_CP_Startup
      {
        PNIO_UINT32 xml_file_size;
        PNIO_UINT32 ret_val;
        PNIO_CP_ID_TYPE locID;
        PNIO_CP_ID_PTR_TYPE CpList;
        PNIO_UINT32 rema_file_size = 0;

        FILE* hXMLFile = NULL;
        FILE* rXMLFile = NULL;

        hXMLFile = fopen(
            "/home/skloe/Coding/no_warning/src/examples/test_app/linux32/"
            "PNDriverBase_TestApp/test.xml",
            "rb");

        rXMLFile = fopen("rema.xml", "rb");  // for rema file

        if (rXMLFile != NULL) {
          fseek(rXMLFile, 0, SEEK_END);
          rema_file_size = ftell(rXMLFile);
          fseek(rXMLFile, 0, SEEK_SET);
          rema_buffer = (char*)malloc(rema_file_size + 1);
          fread(rema_buffer, 1, rema_file_size, (FILE*)rXMLFile);
          rema_buffer[rema_file_size] = 0;  // add terminating NULL
          fclose(rXMLFile);
        }

        if (hXMLFile == NULL) {
          printf(
              "Error: The file 'Station_1.PN "
              "Driver_1.PNDriverConfiguration.xml' cound not be opened!\n");
        } else {
          PNIO_UINT32 result;
          PNIO_UINT8 nrofcp;
          fseek(hXMLFile, 0, SEEK_END);
          xml_file_size = ftell(hXMLFile);
          fseek(hXMLFile, 0, SEEK_SET);
          xml_buffer = (char*)malloc(xml_file_size + 1);
          fread(xml_buffer, 1, xml_file_size, (FILE*)hXMLFile);
          xml_buffer[xml_file_size] = 0;  // add terminating NULL
          fclose(hXMLFile);

          SERV_CP_init(&debSet);

          CpList = (PNIO_CP_ID_PTR_TYPE)malloc(sizeof(PNIO_CP_ID_TYPE) *
                                               PND_MAX_NR_OF_CP);

          memset(&locID, 0, sizeof(PNIO_CP_ID_TYPE));
          memset(CpList, 0, sizeof(PNIO_CP_ID_TYPE) * PND_MAX_NR_OF_CP);

          result = SERV_CP_get_network_adapters(CpList, &nrofcp);

          if (result == PNIO_OK) {
            PNIO_SYSTEM_DESCR sd;

            pnd_test_network_adapter_selection(&locID, CpList, nrofcp);

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

            ret_val = SERV_CP_startup(&locID, 1, (PNIO_UINT8*)xml_buffer,
                                      xml_file_size, (PNIO_UINT8*)rema_buffer,
                                      rema_file_size, &sd);
            if (ret_val == PNIO_ERR_INVALID_REMA) {
              printf("Invalid Rema file!");
            }
            printf("SERV_CP_startup() Result: %lu\n", ret_val);
          }

          if (rXMLFile != NULL) {
            free(rema_buffer);
          }

          free(xml_buffer);

          free(CpList);
        }
      } break;

      case 4:  // SERV_CP_shutdown
      {
        PNIO_UINT32 ret_val;

        ret_val = SERV_CP_shutdown();

        printf("SERV_CP_shutdown() Result:%lu\n", ret_val);

        SERV_CP_undo_init();
      } break;

      case 5:  // open controller
      {
        PNIO_DEBUG_SETTINGS_TYPE debSet;
        debSet.CbfPntrcBufferFull = pnd_test_buffer_full;

        pnd_test_controller_open(&debSet);

        pnd_test_register_setmode_cbf();

        pnd_test_register_devact_cbf();

        pnd_test_register_diag_req_cbf();

        pnd_test_register_iosystem_reconfig();
      } break;

      case 6:  // close controller
      {
        pnd_test_controller_close();
      } break;

      case 7:  // set mode offline
      {
        pnd_test_set_mode(PNIO_MODE_OFFLINE);
      } break;

      case 8:  // set mode clear
      {
        pnd_test_set_mode(PNIO_MODE_CLEAR);
      } break;

      case 9:  // set mode operate
      {
        pnd_test_set_mode(PNIO_MODE_OPERATE);
      } break;

      case 10:  // device activate
      {
        pnd_test_dev_activate(PNIO_DA_TRUE);
      } break;

      case 11:  // device passivate
      {
        pnd_test_dev_activate(PNIO_DA_FALSE);
      } break;

      case 12:  // read I&M Data
      {
        pnd_test_IM_data_read();
      } break;

      case 13:  // write I&M Data
      {
        pnd_test_IM_data_write();
      } break;

      case 15:  // pntrc test
      {
        pnd_test_pntrc();
      } break;

      case 16:  // PNIO_data_write() / PNIO_data_read()
      {
        pnd_test_iodu();
      } break;

      case 0: {
        pnd_test_quit_application(g_ApplHandle);
      } break;
    }
  }

  if (PNTRC_fp) {
    fclose(PNTRC_fp);
  }

  return (ret_val);

}  // end of main

// /*****************************************************************************/
// /*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved. */
// /*****************************************************************************/
