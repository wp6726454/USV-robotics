#include "../include/tcpclient.h"

int main(int argc, char *argv[]) {
  tcpsocketclient s_client = {.sockfd = 0, .numbytes = 0, .rv = 0, .s = ""};

  if (argc != 2) {
    fprintf(stderr, "usage: client hostname\n");
    exit(1);
  }

  setup_caddr_info(&s_client, argv[1]);

  create_socket_and_connect(&s_client);

  get_serveradd_print(&s_client);

  sr_messsages(&s_client);
  close(s_client.sockfd);

  return 0;
}
