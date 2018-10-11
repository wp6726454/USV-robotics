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

#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
#include <conio.h>
#include <iphlpapi.h>
#include <winsock2.h>
#endif

#if defined(EPS_CFG_USE_PNDEVDRV)
#include <PnDev_Driver_Inc.h> /* PnDevDrv Interface   */
#include <precomp.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#include "eps_cp_hw.h"
#include "eps_pn_drv_if.h"

#include "servusrx.h"

#include "pnd_test.h"

#include "pnd_sys.h"

#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
#include <packet32_inc.h> /* EDDS Lower Layer Intel Driver            */
#include "packet32.h"
#include "pcap.h"
#endif

#if (EPS_PLF == EPS_PLF_LINUX_X86)
#include "eps_pndevdrv.h"
#endif

#include <eps_app.h>
#include <eps_plf.h>

#ifdef _MSC_VER
#pragma comment(lib, "IPHLPAPI.lib")
#endif

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))
#define PND_CFG_WPCAPDRV_MAX_BOARDS 50

PNIO_DEBUG_SETTINGS_TYPE dbg;
PNIO_DEBUG_SETTINGS_PTR_TYPE DebugSetting = PNIO_NULL;

#define CHAR_ESC 27
#define CHAR_BACK 8

int is_char_enter(char ch) { return strchr("\r\n", ch) ? 1 : 0; }

void fill_buffer_with_blank_char(PNIO_UINT8 *buffer, PNIO_UINT8 length) {
  PNIO_BOOL bStartFilling = PNIO_FALSE;
  PNIO_UINT8 idx;
  for (idx = 0; idx < length; idx++) {
    if (buffer[idx] == '\0') {
      bStartFilling = PNIO_TRUE;
    }

    if (bStartFilling) {
      buffer[idx] = 0x20;  // Blank character
    }
  }
}

char *get_string_from_stdio(char *buffer, int buffer_length) {
  char ch;
  int i = 0;

  /* Read in single line from "stdin": */
  while ((ch = (char)getchar()) != EOF) {
    if (is_char_enter(ch)) {
      if (i == 0) {
        continue;  // consume ENTER at the beginning
      } else {
        break;  // end of text
      }
    } else if (ch == CHAR_BACK) {
      if (i > 0) {
        i--;
      }
      continue;
    } else if (ch == CHAR_ESC) {
      i = 0;
      break;
    } else if (i < (buffer_length - 1)) {
      buffer[i] = ch;
      i++;
    }
  }

  /* Terminate string with null character: */
  buffer[i] = '\0';
  printf("\n");

  return buffer;
}

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

#if (EPS_PLF == EPS_PLF_WINDOWS_X86)

static HANDLE Console_Event;

void Console_lock_init(void) {
  Console_Event = CreateEvent(NULL, PNIO_FALSE, PNIO_FALSE, NULL);
}

void Console_lock(void) { ResetEvent(Console_Event); }

void Console_unlock(void) { SetEvent(Console_Event); }

void wait_for_con() { WaitForSingleObject(Console_Event, 1000); }

#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
