#include "../../timer/timecounter.hpp"
#include "../include/constants.h"
#include "../include/database.h"
#define CLENGTH(x) (sizeof(x) / sizeof((x)[0]))

void handle_sqlite() {
  timecounter t1("withoutselect");
  mysql sqltest;
  int clientsid[] = {1,  13, 41, 1, 532, 2,  1,    4, 4, 5,  34, 1,
                     23, 5,  34, 5, 1,   53, 5123, 5, 1, 53, 77};

  int n = CLENGTH(clientsid);
  open_database(&sqltest);
  for (int i = 0; i != n; ++i) {
    update_mastertable(&sqltest, clientsid[i]);
    create_client_table(&sqltest, clientsid[i]);
  }

  float testfloat[CLIENT_DATA_SIZE] = {12.00,        232.111111,  25424.3333,
                                       7565,         0.0001,      645.0,
                                       7777.1111111, 1.234556679, 9.1232450};
  // char *str_test = (char *)malloc(CLIENT_DATA_SIZE * 64);
  char str_test[CLIENT_DATA_SIZE * 64] = "test";
  convert_floatp_char(testfloat, str_test, CLIENT_DATA_SIZE);
  printf("%s\n", str_test);
  update_client_table(&sqltest, 1, true, testfloat, CLIENT_DATA_SIZE);
  update_client_table(&sqltest, 1, false, testfloat, CLIENT_DATA_SIZE);
  close_database(&sqltest);
}

int main(int argc, char *argv[]) {
  handle_sqlite();
  return 0;
}
