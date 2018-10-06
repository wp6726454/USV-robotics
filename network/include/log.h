/*
***********************************************************************
* log.h:
* keep a log for this program
*
*  by Hu.ZH(IPAC@SJTU)
***********************************************************************
*/

#ifndef _LOG_H_
#define _LOG_H_
#include <chrono>
#include <fstream>
#include <iomanip>

void Write2LogFile(const std::string &text) {
  std::ofstream log_file("log_file.txt",
                         std::ios_base::out | std::ios_base::app);
  log_file << text << std::endl;
}

void GetCurrentTimeandData(std::string &cs) {
  auto now = std::chrono::system_clock::now();
  auto now_c = std::chrono::system_clock::to_time_t(now);
  std::stringstream ss;
  ss << std::put_time(std::localtime(&now_c), "%Y-%m-%d %X");
  cs = ss.str() + " UTC";
}

#endif /*_LOG_H_*/
