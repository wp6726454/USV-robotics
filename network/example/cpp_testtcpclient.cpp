#include "../include/tcpclient.h"

int main(int argc, char *argv[]) {
  tcpsocketclient s_client = {};

  if (argc != 2) {
    fprintf(stderr, "usage: client hostname\n");
    exit(1);
  }

  setup_caddr_info(&s_client, argv[1]);

  create_socket_and_connect(&s_client);

  get_serveradd_print(&s_client);

  unsigned char buf[] = {'s', 'o', 'c', 'k', 'e', 't', '\n'};
  memcpy(s_client.send_buf, buf, sizeof buf);
  s_client.send_buf[sizeof buf] = '\0';
  while (1) {
    sr_messsages(&s_client);
    s_client.recv_buf[s_client.numbytes] = '\0';
    printf("client: received %s \n", s_client.recv_buf);
    sleep(1);
  }

  close(s_client.sockfd);

  return 0;
}
