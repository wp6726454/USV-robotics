#ifndef _TIMECOUNTER_H_
#define _TIMECOUNTER_H_
#include <boost/date_time/microsec_time_clock.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <iomanip>
#include <iostream>

class timecounter {
  using T_BOOST_CLOCK =
      boost::date_time::microsec_clock<boost::posix_time::ptime>;

public:
  timecounter(const std::string &t_name)
      : name(t_name), t_start(T_BOOST_CLOCK::local_time()){};
  ~timecounter() {
    boost::posix_time::ptime t_now(T_BOOST_CLOCK::local_time());
    boost::posix_time::time_duration t_d = t_now - t_start;
    std::cout.precision(10);
    std::cout << name << " completed in " << std::fixed
              << t_d.total_microseconds() << " us" << std::endl;
  }

private:
  std::string name;
  boost::posix_time::ptime t_start;
};

#endif // _TIMECOUNTER_H_
