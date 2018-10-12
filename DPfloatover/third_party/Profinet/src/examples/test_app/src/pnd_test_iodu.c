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
/*  F i l e               &F: pnd_test_iodu.c                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  IODU test functions for the Test Application for PNDriver                */
/*                                                                           */
/*****************************************************************************/

#include "stdio.h"

#include "pniobase.h"
#include "servusrx.h"

#include <eps_rtos.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "pnd_test.h"

#define TEST_IODU_MAX_DATA_LEN 1024

extern PNIO_UINT32 g_ApplHandle;

typedef struct DataDisplay {
  PNIO_UINT32 RT_Power_1;
  PNIO_UINT32 RT_Power_2;
  PNIO_UINT32 RT_Power_3;
  PNIO_UINT32 RT_Power_4;
  PNIO_UINT32 RT_Power_5;
  PNIO_UINT32 RT_Power_6;
  PNIO_UINT32 RT_ActVelocity_1;
  PNIO_UINT32 RT_ActVelocity_2;
  PNIO_UINT32 RT_ActVelocity_3;
  PNIO_UINT32 RT_ActVelocity_4;
  PNIO_UINT32 RT_Position_5;
  PNIO_UINT32 RT_Position_6;
  PNIO_UINT8 RT_Enabled;
  PNIO_UINT8 RT_Error;
} DDisplay;

void pnd_test_iodu() {
  int cmd = 777;

  while (cmd != 0) {
    printf("----------------------------------------\n");
    printf("1...PLC_1_Control \n");
    printf("2...PLC_2_Control \n");
    printf("3...PLC_3_Control \n");
    printf("4...Data_Display \n");
    printf("5...PNIO_data_write \n");
    printf("6...PNIO_data_read \n");
    printf("7...send_jog \n");
    printf("8...send_20ms_50Hz \n");
    printf("9...send_50ms_20Hz \n");
    printf("0...return to main menu \n");

    scanf("%d", &cmd);

    while (getchar() != '\n')
      ;
    switch (cmd) {
      case 0:
        break;
      case 1:  // PLC_1_Control
      {
        PLC_1_Control();
      } break;
      case 2:  // PLC_2_Control
      {
        PLC_2_Control();
      } break;
      case 3:  // PLC_3_Control
      {
        PLC_3_Control();
      } break;
      case 4:  // Data_Display
      {
        Data_Display();
      } break;
      case 5:  // PNIO_data_write()
      {
        pnd_test_iodu_write();
      } break;
      case 6:  // PNIO_data_read()
      {
        pnd_test_iodu_read();
      } break;
      case 7: {
        send_jog();
        break;
      }
      case 8: {
        send_20ms_50Hz();
        break;
      }
      case 9: {
        send_50ms_20Hz();
        break;
      }
      default:
        break;
    }
  }
}

void pnd_test_iodu_write() {
  PNIO_ADDR Addr;
  PNIO_IOXS remState;
  PNIO_UINT32 response;

  PNIO_UINT32 write_data[TEST_IODU_MAX_DATA_LEN] = {0};

  Addr.AddrType = PNIO_ADDR_LOG;
  Addr.IODataType = PNIO_IO_OUT;

  // printf("----------------------------------------\n");
  // printf("Q Address: ");

  // scanf("%d", (int*)&Addr.u.Addr);

  Addr.u.Addr = 0;

  printf("----------------------------------------\n");
  printf("Q Data: ");

  scanf("%f %f %f %f %f %f", (float *)&write_data[0], (float *)&write_data[1],
        (float *)&write_data[2], (float *)&write_data[3],
        (float *)&write_data[4], (float *)&write_data[5]);

  // call PNIO_data_write
  response = PNIO_data_write(g_ApplHandle, &Addr, 24 /*BufLen*/, write_data,
                             PNIO_S_GOOD, &remState);

  printf("\nPNIO_data_write() response: 0x%x remState: 0x%x\n",
         (unsigned int)response, (unsigned int)remState);
}

void pnd_test_iodu_read() {
  PNIO_ADDR Addr;
  PNIO_IOXS remState;
  PNIO_UINT32 response;
  PNIO_UINT32 dataLen;

  PNIO_UINT8 read_data[TEST_IODU_MAX_DATA_LEN] = {0};

  Addr.AddrType = PNIO_ADDR_LOG;
  Addr.IODataType = PNIO_IO_IN;

  printf("----------------------------------------\n");
  printf("I Address: ");

  scanf("%d", (int *)&Addr.u.Addr);

  // call PNIO_data_read
  response = PNIO_data_read(g_ApplHandle, &Addr, 1 /*BufLen*/, &dataLen,
                            read_data, PNIO_S_GOOD, &remState);

  printf("\n");
  printf("I Data: 0x%x  remState: %d (0=GOOD, 1=BAD)\n", read_data[0],
         remState);

  printf("\nPNIO_data_read() response: 0x%x\n", (unsigned int)response);
}

void PLC_1_Control() {
  PNIO_ADDR Addr;
  PNIO_IOXS remState;
  PNIO_UINT32 response;

  PNIO_UINT32 write_data[TEST_IODU_MAX_DATA_LEN] = {0};

  Addr.AddrType = PNIO_ADDR_LOG;
  Addr.IODataType = PNIO_IO_OUT;

  Addr.u.Addr = 0;

  printf("-----------------PLC_1_Control-----------------\n");
  printf("Data: ");

  scanf("%f %f %f %f %f %f", (float *)&write_data[0], (float *)&write_data[1],
        (float *)&write_data[2], (float *)&write_data[3],
        (float *)&write_data[4], (float *)&write_data[5]);

  // call PNIO_data_write
  response = PNIO_data_write(g_ApplHandle, &Addr, 24 /*BufLen*/, write_data,
                             PNIO_S_GOOD, &remState);

  printf("\nPNIO_data_write() response: 0x%x remState: 0x%x\n",
         (unsigned int)response, (unsigned int)remState);
}

void PLC_2_Control() {
  PNIO_ADDR Addr;
  PNIO_IOXS remState;
  PNIO_UINT32 response;

  PNIO_UINT32 write_data[TEST_IODU_MAX_DATA_LEN] = {0};

  Addr.AddrType = PNIO_ADDR_LOG;
  Addr.IODataType = PNIO_IO_OUT;

  Addr.u.Addr = 24;

  printf("-----------------PLC_2_Control-----------------\n");
  printf("Data: ");

  scanf("%f %f %f %f %f %f", (float *)&write_data[0], (float *)&write_data[1],
        (float *)&write_data[2], (float *)&write_data[3],
        (float *)&write_data[4], (float *)&write_data[5]);

  // call PNIO_data_write
  response = PNIO_data_write(g_ApplHandle, &Addr, 24 /*BufLen*/, write_data,
                             PNIO_S_GOOD, &remState);

  printf("\nPNIO_data_write() response: 0x%x remState: 0x%x\n",
         (unsigned int)response, (unsigned int)remState);
}

void PLC_3_Control() {
  PNIO_ADDR Addr;
  PNIO_IOXS remState;
  PNIO_UINT32 response;

  PNIO_UINT32 write_data[TEST_IODU_MAX_DATA_LEN] = {0};

  Addr.AddrType = PNIO_ADDR_LOG;
  Addr.IODataType = PNIO_IO_OUT;

  Addr.u.Addr = 50;

  printf("-----------------PLC_3_Control-----------------\n");
  printf("Data: ");

  scanf("%f %f %f %f %f %f", (float *)&write_data[0], (float *)&write_data[1],
        (float *)&write_data[2], (float *)&write_data[3],
        (float *)&write_data[4], (float *)&write_data[5]);

  // call PNIO_data_write
  response = PNIO_data_write(g_ApplHandle, &Addr, 24 /*BufLen*/, write_data,
                             PNIO_S_GOOD, &remState);

  printf("\nPNIO_data_write() response: 0x%x remState: 0x%x\n",
         (unsigned int)response, (unsigned int)remState);
}

void Data_Display() {
  DDisplay *DD1;
  DDisplay *DD2;
  PNIO_ADDR Addr;
  PNIO_IOXS remState;
  PNIO_UINT32 response;
  PNIO_UINT32 dataLen;

  float read_data_PLC_1[50] = {0};
  float read_data_PLC_2[50] = {0};

  Addr.AddrType = PNIO_ADDR_LOG;
  Addr.IODataType = PNIO_IO_IN;

  Addr.u.Addr = 24;

  // call PNIO_data_read
  response = PNIO_data_read(g_ApplHandle, &Addr, 24 /*BufLen*/, &dataLen,
                            read_data_PLC_1, PNIO_S_GOOD, &remState);

  DD1 = read_data_PLC_1;

  printf("\n");
  printf("PLC_1_Power:\n");
  printf("%x  %.2f  %.2f  %.2f  %.2f  %.2f  %.2f \n", DD1->RT_Power_1,
         (float *)&DD1->RT_Power_1, (float)DD1->RT_Power_2,
         (float)DD1->RT_Power_3, (float)DD1->RT_Power_4, (float)DD1->RT_Power_5,
         (float)DD1->RT_Power_6);
  printf("PLC_1_Vel & Pos:\n");
  printf("%.2f  %.2f  %.2f  %.2f  %.2f  %.2f \n", (float)DD1->RT_ActVelocity_1,
         (float)DD1->RT_ActVelocity_2, (float)DD1->RT_ActVelocity_3,
         (float)DD1->RT_ActVelocity_4, (float)DD1->RT_Position_5,
         (float)DD1->RT_Position_6);
  printf("remState: %d (0=GOOD, 1=BAD)\n", remState);

  // printf("\n");
  // printf("I Data: %x   %.2f  remState: %d (0=GOOD, 1=BAD)\n",
  //        read_data_PLC_1[0], (float)read_data_PLC_1[0], remState);
  printf("\nPNIO_data_read() response: 0x%x\n", (unsigned int)response);

  // (int *)Addr.u.Addr = 50;

  // // call PNIO_data_read
  // response = PNIO_data_read(g_ApplHandle, &Addr, 50 /*BufLen*/, &dataLen,
  //                           read_data_PLC_2, PNIO_S_GOOD, &remState);

  // printf("\n");
  // printf("I Data: 0x%x  remState: %d (0=GOOD, 1=BAD)\n", read_data_PLC_2[0],
  //        remState);

  // printf("\nPNIO_data_read() response: 0x%x\n", (unsigned int)response);
}

void send_jog() {  // 10s per circle, 10000ms/20ms = 500
  PNIO_ADDR Addr;
  PNIO_IOXS remState;
  PNIO_UINT32 response;

  float write_data[TEST_IODU_MAX_DATA_LEN] = {0};

  Addr.AddrType = PNIO_ADDR_LOG;
  Addr.IODataType = PNIO_IO_OUT;

  Addr.u.Addr = 0;

  float i = 0;

  printf("-----------------Send Jog-----------------\n");
  printf("Data: ");

  write_data[0] = 0;
  write_data[1] = 10;
  write_data[2] = 20;
  write_data[3] = 30;
  write_data[4] = 90;
  write_data[5] = -90;
  response = PNIO_data_write(g_ApplHandle, &Addr, 24 /*BufLen*/, write_data,
                             PNIO_S_GOOD, &remState);

  printf("\nPNIO_data_write() response: 0x%x remState: 0x%x\n",
         (unsigned int)response, (unsigned int)remState);
}

void send_20ms_50Hz() {  // 10s per circle, 10000ms/20ms = 500
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

void send_50ms_20Hz() {
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
