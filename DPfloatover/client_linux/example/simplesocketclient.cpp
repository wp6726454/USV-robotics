#include "../../network/include/crccheck.h"
#include "../../network/include/datapack.h"
#include "../include/tcpclient.h"
#include "/home/scar1et/Coding/CPP1X/USV/timer/timecounter.hpp"
#include <cfloat>
#include <iostream>
int main(int argc, char *argv[]) {
  tcpsocketclient s_client = {};

  if (argc != 2) {
    fprintf(stderr, "usage: client hostname\n");
    exit(1);
  }

  setup_caddr_info(&s_client, argv[1]);

  create_socket_and_connect(&s_client);

  get_serveradd_print(&s_client);

  unsigned char buf[MAXDATASIZE];
  double test_float[CLIENT_DATA_SIZE] = {12.00,        232.1111,    25424.3333,
                                         7565,         0.0001,      645.0,
                                         7777.1111111, 1.234556679, 9.123245};
  for (int i = 0; i != CLIENT_DATA_SIZE; ++i)
    printf("%f\n", test_float[i]);

  int count = 0;
  while (1) {
    packfloat32array(buf, test_float, CLIENT_DATA_SIZE);

    append_crc16_checksum(buf, MAXDATASIZE);
    // packi16(buf + 1, packetsize); // store packet size in packet for kicks
    memcpy(s_client.send_buf, buf, MAXDATASIZE);
    // s_client.send_buf[packetsize] = '\0';
    double untest_float[CLIENT_DATA_SIZE] = {};
    sr_messsages(&s_client);
    if (verify_crc16_checksum(s_client.recv_buf, MAXDATASIZE)) {
      unpackfloat32array(s_client.recv_buf, untest_float, CLIENT_DATA_SIZE);
      printf("%f %f %f %f %f %f %f %f %f\n", untest_float[0], untest_float[1],
             untest_float[2], untest_float[3], untest_float[4], untest_float[5],
             untest_float[6], untest_float[7], untest_float[8]);
      printf("%d\n", count);
      ++count;
      // if (test64 != dtest)
      //   std::cout << DBL_MAX << std::endl;
    } else {
      printf("crc failed!\n");
    }
    // usleep(1000); // 10000 milliseconds
  }

  close(s_client.sockfd);

  return 0;
}
