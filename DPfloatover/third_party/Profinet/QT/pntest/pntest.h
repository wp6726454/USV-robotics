#ifndef PNTEST_H
#define PNTEST_H

#define _CRTDBG_MAP_ALLOC
#include <psi_cfg.h>
#include <stdlib.h>

#define LTRC_ACT_MODUL_ID 0000
#define PND_MODULE_ID 0000

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "pnd_int.h"
#include "pnd_test.h"
#include "pniobase.h"
#include "servusrx.h"

static FILE* PNTRC_fp = 0;
static PNIO_DEBUG_SETTINGS_TYPE debSet;
extern PNIO_UINT32 g_ApplHandle;

#define PND_MAX_NR_OF_CP 50

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

PNIO_UINT32 _SERV_CP_shutdown() {
  PNIO_UINT32 ret_val = 1;

  ret_val = SERV_CP_shutdown();

  SERV_CP_undo_init();
  return ret_val;
}

PNIO_UINT32 _SERV_CP_Startup() {
  char* xml_buffer = 0;
  PNIO_UINT32 xml_file_size;
  PNIO_UINT32 ret_val = 1;
  PNIO_CP_ID_TYPE locID;
  PNIO_CP_ID_PTR_TYPE CpList;
  PNIO_UINT32 rema_file_size = 0;

  FILE* hXMLFile = NULL;

  hXMLFile = fopen(
      "/home/skloe/Coding/no_warning/src/examples/test_app/linux32/"
      "PNDriverBase_TestApp/test.xml",
      "rb");

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

    CpList =
        (PNIO_CP_ID_PTR_TYPE)malloc(sizeof(PNIO_CP_ID_TYPE) * PND_MAX_NR_OF_CP);

    memset(&locID, 0, sizeof(PNIO_CP_ID_TYPE));
    memset(CpList, 0, sizeof(PNIO_CP_ID_TYPE) * PND_MAX_NR_OF_CP);

    result = SERV_CP_get_network_adapters(CpList, &nrofcp);

    if (result == PNIO_OK) {
      PNIO_SYSTEM_DESCR sd;

      my_pnd_test_network_adapter_selection(&locID, CpList, nrofcp, stdout);

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
                                xml_file_size, NULL, rema_file_size, &sd);
      if (ret_val == PNIO_ERR_INVALID_REMA) {
        printf("Invalid Rema file!");
      }
      printf("SERV_CP_startup() Result: %lu\n", ret_val);
    }
    free(xml_buffer);
    free(CpList);
  }
  return ret_val;
}

void _opencontroller() {
  PNIO_DEBUG_SETTINGS_TYPE debSet;
  debSet.CbfPntrcBufferFull = pnd_test_buffer_full;

  pnd_test_controller_open(&debSet);

  pnd_test_register_setmode_cbf();

  pnd_test_register_devact_cbf();

  pnd_test_register_diag_req_cbf();

  pnd_test_register_iosystem_reconfig();
}

void pn_send_test() {
  pnd_init();
  if (_SERV_CP_Startup() == 0) {
    _opencontroller();
    pnd_test_set_mode(PNIO_MODE_OPERATE);

    sleep(10);
    // send_50ms_20Hz();//PLC2
    while (1) {
      send_20ms_50Hz();  // PLC1
    }
  }
  pnd_test_controller_close();
  pnd_test_quit_application(g_ApplHandle);
}
#endif  // PNTEST_H
