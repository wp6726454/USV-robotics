// Fixing https://github.com/aminroosta/sqlite_modern_cpp/issues/63
#include <cstdlib>
#include <iostream>
#include <memory>
#include <sqlite_modern_cpp.h>
using namespace sqlite;
using namespace std;

struct dbFront {
  std::unique_ptr<database_binder> storedProcedure;
  database db;
  dbFront() : db(":memory:") {
    db << "CREATE TABLE tbl (id integer, name string);";
    // the temporary moved object should not run _execute() function on
    // destruction.
    storedProcedure = std::make_unique<database_binder>(
        db << "INSERT INTO tbl VALUES (?, ?);");
  }
};

int main() {

  try {
    dbFront dbf;
  } catch (sqlite_exception e) {
    cout << "Unexpected error " << e.what() << endl;
    exit(EXIT_FAILURE);
  } catch (...) {
    cout << "Unknown error\n";
    exit(EXIT_FAILURE);
  }

  cout << "OK\n";
  exit(EXIT_SUCCESS);
}
