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
/*  F i l e               &F: pnd_test_helper.c                         :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Helper functions for the Test Application for PNDriver                   */
/*                                                                           */
/*****************************************************************************/
#include "eps_sys.h"

#if defined(EPS_CFG_USE_PNDEVDRV)
#include <PnDev_Driver_Inc.h> /* PnDevDrv Interface   */
#include <precomp.h>
#endif

#include <eps_app.h>
#include <eps_plf.h>
#include <stdio.h>
#include <stdlib.h>
#include "eps_cp_hw.h"
#include "eps_pn_drv_if.h"
#include "eps_pndevdrv.h"
#include "pnd_sys.h"
#include "pnd_test.h"
#include "servusrx.h"

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))
#define PND_CFG_WPCAPDRV_MAX_BOARDS 50

PNIO_DEBUG_SETTINGS_TYPE dbg;
PNIO_DEBUG_SETTINGS_PTR_TYPE DebugSetting = PNIO_NULL;

#define CHAR_ESC 27
#define CHAR_BACK 8

extern PNIO_UINT32 g_ApplHandle;

void my_pnd_test_network_adapter_selection(PNIO_CP_ID_TYPE *cp_id,
                                           PNIO_CP_ID_PTR_TYPE cp_list,
                                           PNIO_UINT8 nrofcp, FILE *_file) {
  PNIO_CP_ID_PTR_TYPE pCpList = cp_list;
  PNIO_CP_ID_PTR_TYPE pDevice;

  if (nrofcp == 0) {
    fprintf(_file, "\nNo network adapter found!");
    return;
  }

  pDevice = (PNIO_CP_ID_PTR_TYPE)&pCpList[0];

  if (pDevice->CpSelection == PNIO_CP_SELECT_WITH_MAC_ADDRESS) {
    fprintf(_file, "\r\n %d ... %02x:%02x:%02x:%02x:%02x:%02x  -  %s", 0,
            pDevice->CpMacAddr[0], pDevice->CpMacAddr[1], pDevice->CpMacAddr[2],
            pDevice->CpMacAddr[3], pDevice->CpMacAddr[4], pDevice->CpMacAddr[5],
            pDevice->Description);
  }
  // only one network board
  memcpy(cp_id, &pCpList[0], sizeof(PNIO_CP_ID_TYPE));
}

void pnd_test_network_adapter_selection(PNIO_CP_ID_TYPE *cp_id,
                                        PNIO_CP_ID_PTR_TYPE cp_list,
                                        PNIO_UINT8 nrofcp) {
  PNIO_UINT8 i;
  PNIO_CP_ID_PTR_TYPE pCpList = cp_list;
  PNIO_CP_ID_PTR_TYPE pDevice;
  int cmd = 777;

  if (nrofcp == 0) {
    printf("\nNo network adapter found!");
    return;
  }

  printf("\r\nFound network adapters:\r\n");
  printf("----------------------------------------");

  for (i = 0; i < nrofcp; i++) {
    pDevice = (PNIO_CP_ID_PTR_TYPE)&pCpList[i];

    if (pDevice->CpSelection == PNIO_CP_SELECT_WITH_MAC_ADDRESS) {
      printf("\r\n %d ... %02x:%02x:%02x:%02x:%02x:%02x  -  %s", i,
             pDevice->CpMacAddr[0], pDevice->CpMacAddr[1],
             pDevice->CpMacAddr[2], pDevice->CpMacAddr[3],
             pDevice->CpMacAddr[4], pDevice->CpMacAddr[5],
             pDevice->Description);
    } else if (pDevice->CpSelection == PNIO_CP_SELECT_WITH_PCI_LOCATION) {
      printf("\r\n %d ... Bus %2d, Dev %2d, Fct %2d - %s", i,
             pDevice->CpPciLocation.BusNr, pDevice->CpPciLocation.DeviceNr,
             pDevice->CpPciLocation.FunctionNr, pDevice->Description);
    }
  }

  printf("\r\n----------------------------------------\r\n");

  while (cmd < 0 || cmd >= nrofcp) {
    printf("\nEnter the number of the network adapter: ");
    scanf_s("%d", &cmd);
  }

  memcpy(cp_id, &pCpList[cmd], sizeof(PNIO_CP_ID_TYPE));
}

void pnd_test_quit_application(PNIO_UINT32 handle) {
  PNIO_interface_close(handle);
  PNIO_controller_close(handle);
  SERV_CP_shutdown();
  SERV_CP_undo_init();
}

void send_20ms_50Hz(void) {  // 10s per circle, 10000ms/20ms = 500
  PNIO_ADDR Addr;
  PNIO_IOXS remState;
  PNIO_UINT32 response;

  float write_data[TEST_IODU_MAX_DATA_LEN];

  Addr.AddrType = PNIO_ADDR_LOG;
  Addr.IODataType = PNIO_IO_OUT;

  Addr.u.Addr = 0;

  float i = 0;
  int j = 0;
  int count = 0;

  printf("PLC_1_Control:\n");
  printf("send_data>>>>>>>>>>>>>>>>>>\n");

  for (j = 0; j < 2; j++) {
    for (i = 0; i < 360.0; i += 0.72) {
      write_data[0] = 0;
      write_data[1] = 0;
      write_data[2] = 0;
      write_data[3] = 0;
      write_data[4] = i;
      write_data[5] = i;
      response = PNIO_data_write(g_ApplHandle, &Addr, 24 /*BufLen*/, write_data,
                                 PNIO_S_GOOD, &remState);
      if (remState != 0x00) count++;
      usleep(50000);
    }
    printf(">>%d * 500\n", j + 1);
  }
  printf("count = %d\n", count);
  printf("package loss rate: %.8f%\n", ((float)count) / 100.0);
}

void send_50ms_20Hz(void) {
  PNIO_ADDR Addr;
  PNIO_IOXS remState;
  PNIO_UINT32 response;

  float write_data[TEST_IODU_MAX_DATA_LEN];

  Addr.AddrType = PNIO_ADDR_LOG;
  Addr.IODataType = PNIO_IO_OUT;

  Addr.u.Addr = 24;

  float i = 0;
  int j = 0;
  int count = 0;

  printf("PLC_2_Control:\n");
  printf("send_data>>>>>>>>>>>>>>>>>>\n");

  for (j = 0; j < 2; j++) {
    for (i = 0; i < 360.0; i += 0.72) {
      write_data[0] = 20;
      write_data[1] = 20;
      write_data[2] = 20;
      write_data[3] = 20;
      write_data[4] = i;
      write_data[5] = i;
      response = PNIO_data_write(g_ApplHandle, &Addr, 24 /*BufLen*/, write_data,
                                 PNIO_S_GOOD, &remState);
      if (remState != 0x00) count++;
      usleep(50000);
    }
    printf(">>%d * 500\n", j + 1);
  }
  printf("count = %d\n", count);
  printf("package loss rate: %.8f%\n", ((float)count) / 100.0);
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
