#include "savefile.h"

int main() {
  myfile test;
  test.openfile();
  test.savefile();
  test.closefile();
  return 0;
}
