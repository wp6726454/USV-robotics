/*
***********************************************************************
* setpoints.h:
* function for real time setpoints for controller
* This header file can be read by C++ compilers
*
* by Hu.ZH(Mr.SJTU)
***********************************************************************
*/
#ifndef _SETPOINTS_H_
#define _SETPOINTS_H_

#include <Eigen/Core>
#include <Eigen/Dense>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/timer.hpp>
#include <chrono>
#include <cmath>
#include <thread>
#include "constants.h"
#include "realtimedata.h"

// go to fixed point
struct fixedpointdata {
  double desired_theta;
  double desired_finalx;
  double desired_finaly;
};

// go straight line
struct strightlinedata {
  double desired_velocity;
  double desired_theta;
  double desired_finalx;
  double desired_finaly;
  double initialx;
  double initialy;

  int orientation_adjustment_time;  // elapsed time for orientation
                                    // adjustment(seconds)
};

class setpoints {
 public:
  setpoints() {}
  ~setpoints() {}
  void gofixedpoint_first(realtimevessel_first &_realtimevessel,
                          const fixedpointdata &_fixedpointdata) {
    gofixedpoint(_realtimevessel.setPoints, _fixedpointdata);
  }
  void gofixedpoint_second(realtimevessel_second &_realtimevessel,
                           const fixedpointdata &_fixedpointdata) {
    gofixedpoint(_realtimevessel.setPoints, _fixedpointdata);
  }
  void gofixedpoint_third(realtimevessel_third &_realtimevessel,
                          const fixedpointdata &_fixedpointdata) {
    gofixedpoint(_realtimevessel.setPoints, _fixedpointdata);
  }
  void gostraightline_first(realtimevessel_first &_realtimevessel,
                            const strightlinedata &_strightlinedata) {
    gostraightline(_realtimevessel.setPoints, _strightlinedata);
  }
  void gostraightline_second(realtimevessel_second &_realtimevessel,
                             const strightlinedata &_strightlinedata) {
    gostraightline(_realtimevessel.setPoints, _strightlinedata);
  }
  void gostraightline_third(realtimevessel_third &_realtimevessel,
                            const strightlinedata &_strightlinedata) {
    gostraightline(_realtimevessel.setPoints, _strightlinedata);
  }

 private:
  Eigen::Vector3d setpoint_first;
  Eigen::Vector3d setpoint_second;
  Eigen::Vector3d setpoint_third;

  void gofixedpoint(Eigen::Vector3d &_setpoints,
                    const fixedpointdata &_fixedpointdata) {
    _setpoints << _fixedpointdata.desired_finalx,
        _fixedpointdata.desired_finaly, _fixedpointdata.desired_theta;
  }

  // we keep the orientation and velocity to go a straight line, to a final
  // points
  void gostraightline(Eigen::Vector3d &_setpoints,
                      const strightlinedata &_strightlinedata) {
    // setup timer
    boost::posix_time::ptime t_start =
        boost::posix_time::second_clock::local_time();
    boost::posix_time::ptime t_end =
        boost::posix_time::second_clock::local_time();
    boost::posix_time::time_duration t_elapsed = t_end - t_start;
    long int mt_elapsed = 0;

    // We reach the desired orientation first.
    _setpoints << _strightlinedata.initialx, _strightlinedata.initialy,
        _strightlinedata.desired_theta;
    std::this_thread::sleep_for(
        std::chrono::seconds(_strightlinedata.orientation_adjustment_time));

    // then we keep the straight line and reach the desired points
    double total_delta_x =
        _strightlinedata.desired_finalx - _strightlinedata.initialx;
    double total_delta_y =
        _strightlinedata.desired_finaly - _strightlinedata.initialy;
    double total_length = std::sqrt(total_delta_x * total_delta_x +
                                    total_delta_y * total_delta_y);
    long int total_mt_elapsed =
        (long int)(1000 * total_length / _strightlinedata.desired_velocity);
    // update the desired position step by step
    do {
      t_end = boost::posix_time::second_clock::local_time();
      t_elapsed = t_end - t_start;
      mt_elapsed = t_elapsed.total_milliseconds();
      _setpoints(0) = total_delta_x * mt_elapsed / total_mt_elapsed +
                      _strightlinedata.initialx;
      _setpoints(1) = total_delta_y * mt_elapsed / total_mt_elapsed +
                      _strightlinedata.initialy;
      _setpoints(2) = _strightlinedata.desired_theta;
    } while (mt_elapsed < total_mt_elapsed)
  }
};
#endif /* _SETPOINTS_H_ */