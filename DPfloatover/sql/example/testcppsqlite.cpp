#include "../include/databasecpp.h"
int main() {
  try {
    // creates a database file 'dbfile.db' if it does not exists.
    databasecpp mydb;
    mydb.create_mastertable();
    for (int i = 0; i != MAXCONNECTION; ++i) {
      mydb.update_mastertable(i);
      mydb.create_client_table(i);
    }
    Eigen::Matrix<double, 9, 1> test;
    test << 1, 2, 3, 4, 5, 6, 7, 8, 9;
    for (int i = 0; i != MAXCONNECTION; ++i) {
      mydb.update_client_table(i, 1, test);
      mydb.update_client_table(i, 0, test);
    }
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
  }
}
