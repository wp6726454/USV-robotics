#include <iostream>
#include <memory>
#include <sqlite_modern_cpp.h>
#include <stdexcept>
#include <string>
using namespace sqlite;
using namespace std;

struct A {
  ~A() {
    database db(":memory:");
    db << "CREATE TABLE person (id integer primary key not null, name TEXT not "
          "null);";

    try {
      auto stmt = db << "INSERT INTO person (id,name) VALUES (?,?)";
      throw 1;
    } catch (int) {
    }
  }
};
int main() {
#ifdef __cpp_lib_uncaught_exceptions
  try {
    A a;
    throw 1;
  } catch (int) {
  }
  exit(EXIT_SUCCESS);
#else
  exit(42);
#endif
}
