#include "../include/tcpserver.h"

int main(void) {
  tcpsocketserver t_server = {};
  setup_saddr_info(&t_server);
  open_listenfd(&t_server);
  while (1) {
    select_rvmessage(&t_server);
  }

  return 0;
}
