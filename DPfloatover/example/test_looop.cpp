#include "../include/Loop.h"
#include <iostream>
#include <string>

int main() {

  looop myloop;
  myloop.initializelooop();

  myloop.start_connnection_t();

  while (1) {
    std::cout << myloop.get_connection_status() << std::endl;
    if (myloop.get_connection_status()) {
      myloop.receiveallclients_t();
      break;
    }
  }

  myloop.closelooop();

  return 0;
}
