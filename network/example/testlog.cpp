#include "../include/log.h"

int main() {
  Write2LogFile("Initializaion of Raspberry Pi GPIO is Done!");
  std::string t_cs;

  GetCurrentTimeandData(t_cs);
  Write2LogFile(t_cs);
}
