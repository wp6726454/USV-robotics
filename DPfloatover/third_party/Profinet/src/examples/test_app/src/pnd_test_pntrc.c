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
/*  F i l e               &F: pnd_test_pntrc.c                          :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  PNTrace functions for the Test Application for PNDriver                  */
/*                                                                           */
/*****************************************************************************/

#include "stdio.h"

#include "pniobase.h"
#include "servusrx.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pnd_test.h"

void pnd_test_set_trace_level(PNIO_UINT32 component);

void pnd_test_pntrc() {
  char cmd = 'a';
  PNIO_UINT32 component = 0;

  while (cmd != 'z') {
    printf("----------------------------------------\n");
    printf("Set Trace Level \n");
    printf("----------------------------------------\n");
    printf("Select Component: \n");
    printf("\n");
    printf("a...ACP\n");
    printf("b...CLRPC\n");
    printf("c...CM\n");
    printf("d...DCP\n");
    printf("e...EDDS\n");
    printf("f...EPS\n");
    printf("g...HIF\n");
    printf("h...LLDP\n");
    printf("i...NARE\n");
    printf("j...OHA\n");
    printf("k...PNTRC\n");
    printf("l...PSI\n");
    printf("m...SOCK\n");
    printf("n...TCIP\n");
    printf("o...PND\n");
    printf("\n");
    printf("z...QUIT \n");

    scanf("%c", &cmd);

    while (getchar() != '\n')
      ;

    switch (cmd) {
      case 'a': {
        component = PNIO_TRACE_COMP_ACP;
      } break;
      case 'b': {
        component = PNIO_TRACE_COMP_CLRPC;
      } break;
      case 'c': {
        component = PNIO_TRACE_COMP_CM;
      } break;
      case 'd': {
        component = PNIO_TRACE_COMP_DCP;
      } break;
      case 'e': {
        component = PNIO_TRACE_COMP_EDDS;
      } break;
      case 'f': {
        component = PNIO_TRACE_COMP_EPS;
      } break;
      case 'g': {
        component = PNIO_TRACE_COMP_HIF;
      } break;
      case 'h': {
        component = PNIO_TRACE_COMP_LLDP;
      } break;
      case 'i': {
        component = PNIO_TRACE_COMP_NARE;
      } break;
      case 'j': {
        component = PNIO_TRACE_COMP_OHA;
      } break;
      case 'k': {
        component = PNIO_TRACE_COMP_PNTRC;
      } break;
      case 'l': {
        component = PNIO_TRACE_COMP_PSI;
      } break;
      case 'm': {
        component = PNIO_TRACE_COMP_SOCK;
      } break;
      case 'n': {
        component = PNIO_TRACE_COMP_TCIP;
      } break;
      case 'o': {
        component = PNIO_TRACE_COMP_PND;
      } break;

      default:
        // loop again
        break;
    }  // switch

    if (cmd >= 'a' && cmd <= 'o') {
      pnd_test_set_trace_level(component);
    }
  }
}

void pnd_test_set_trace_level(PNIO_UINT32 component) {
  char cmd = '0';
  PNIO_UINT32 tracelevel = 0;
  PNIO_UINT32 ret_val;

  while (cmd != 'z' && !(cmd >= 'a' && cmd <= 'i')) {
    printf("\n");
    printf("Select Trace Level: \n");
    printf("\n");
    printf("a...OFF\n");
    printf("b...FATAL\n");
    printf("c...ERROR\n");
    printf("d...UNEXP\n");
    printf("e...WARN\n");
    printf("f...NOTE_HIGH\n");
    printf("g...NOTE\n");
    printf("h...NOTE_LOW\n");
    printf("i...CHAT\n");
    printf("\n");
    printf("z...QUIT \n");

    scanf("%c", &cmd);

    while (getchar() != '\n')
      ;

    switch (cmd) {
      case 'a': {
        tracelevel = PNIO_TRACE_LEVEL_OFF;
      } break;
      case 'b': {
        tracelevel = PNIO_TRACE_LEVEL_FATAL;
      } break;
      case 'c': {
        tracelevel = PNIO_TRACE_LEVEL_ERROR;
      } break;
      case 'd': {
        tracelevel = PNIO_TRACE_LEVEL_UNEXP;
      } break;
      case 'e': {
        tracelevel = PNIO_TRACE_LEVEL_WARN;
      } break;
      case 'f': {
        tracelevel = PNIO_TRACE_LEVEL_NOTE_HIGH;
      } break;
      case 'g': {
        tracelevel = PNIO_TRACE_LEVEL_NOTE;
      } break;
      case 'h': {
        tracelevel = PNIO_TRACE_LEVEL_NOTE_LOW;
      } break;
      case 'i': {
        tracelevel = PNIO_TRACE_LEVEL_CHAT;
      } break;

      default:
        // loop again
        break;
    }  // switch

    if (cmd >= 'a' && cmd <= 'i') {
      ret_val = SERV_CP_set_trace_level(component, tracelevel,
                                        pnd_test_set_trace_level_done);
      printf("SERV_CP_set_trace_level(): PNIO result:%u\n",
             (unsigned int)ret_val);
    }
  }
}

void pnd_test_set_trace_level_done(void) {
  printf("\nSERV_CP_set_trace_level_done()!\n");
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
