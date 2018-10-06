#include "../include/crccheck.h"
#include "../include/datapack.h"
#include "../include/tcpclient.h"
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

  unsigned char buf[100];
  const char *s = "Great unmitigated Zot!  You've found the Runestaff!";
  unsigned int packetsize, ps2;
  long double test64 = 53435.1111223553603242;

  while (1) {
    unsigned char magic;
    int monkeycount;
    long altitude;
    double absurdityfactor;
    char s2[96];
    uint64_t Qtest;
    long double dtest;

    packetsize = pack(buf, "CHhlsdQg", 'B', 0, 37, 0x7fffffffl, s, -3490.53555,
                      9007199254740991ll, test64);
    append_crc16_checksum(buf, sizeof buf);
    // packi16(buf + 1, packetsize); // store packet size in packet for kicks
    memcpy(s_client.send_buf, buf, 100);
    // s_client.send_buf[packetsize] = '\0';

    sr_messsages(&s_client);
    if (verify_crc16_checksum(s_client.recv_buf, 100)) {
      unpack(s_client.recv_buf, "CHhlsdQg", &magic, &ps2, &monkeycount,
             &altitude, s2, &absurdityfactor, &Qtest, &dtest);
      printf("'%c' %hhu %u %ld \"%s\" %f %lld %Lf\n", magic, ps2, monkeycount,
             altitude, s2, absurdityfactor, Qtest, dtest);
      if (test64 != dtest)
        std::cout << DBL_MAX << std::endl;
    }

    sleep(1);
  }

  close(s_client.sockfd);

  return 0;
}
