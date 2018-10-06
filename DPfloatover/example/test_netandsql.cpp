// #include "../../network/include/log.h"
#include "../include/loop.h"

int main(int argc, char *argv[]) {
  mysql sqltest;
  tcpsocketserver t_server = {};
  // static tcpsocketserver t_server = {};

  // initialize socket server
  setup_saddr_info(&t_server);
  open_listenfd(&t_server);

  // initialize sqlite database
  open_database(&sqltest);

  while (1) {
    if (select_accept(&t_server, &sqltest))
      break;
  }
  for (int i = 0; i != 5000; ++i) {
    receiveallclients(&t_server, &sqltest);
  }
  // while (1) {
  //   receiveallclients(&t_server, &sqltest);
  // }
  close_database(&sqltest);
  return 0;
}
