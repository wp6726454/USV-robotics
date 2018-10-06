/*
***********************************************************************
* motioncapture.hpp:
* real-time SDK for QTM, which could enable real-time motion capture.
* This header file can be read by C++ compilers
*
*  by Hu.ZH(Mr.SJTU)
***********************************************************************
*/

#ifndef _MOTIONCAPTURE_H
#define _MOTIONCAPTURE_H

#include <math.h>
#include <stdio.h>
#include <Eigen/Core>
#include <chrono>
#include <iostream>
#include <thread>
#include "constants.h"
#include "realtimedata.h"

typedef Eigen::Matrix<double, 6, MAXCONNECTION> Mat4Allclient_6DoF;

class motioncapture {
 public:
  explicit motioncapture(int index_default = 1)
      : acquisitionindex(index_default) {}
  ~motioncapture() {}
  // request and update the motion data, this function could be called by
  // multi-thread
  void RequestPositionandVelocity(realtimevessel_first &_realtimevessel_first,
                                  realtimevessel_second &_realtimevessel_second,
                                  realtimevessel_third &_realtimevessel_third) {
    if (MAXCONNECTION > 0) getcurrentmotion_first(_realtimevessel_first);
    if (MAXCONNECTION > 1) getcurrentmotion_second(_realtimevessel_second);
    if (MAXCONNECTION > 2) getcurrentmotion_third(_realtimevessel_third);
  }

 private:
  // raw data

  // data acquisition successful or not
  int acquisitionindex;

  /* QTMclient API */
  // get the current position and velocity of the first vessel (K class-I)
  void getcurrentmotion_first(realtimevessel_first &_realtimevessel_first) {
    _realtimevessel_first.Position.setRandom();
    _realtimevessel_first.Velocity.setRandom();
    _realtimevessel_first.Measurement.setRandom();
    static int angle = 0;
    _realtimevessel_first.Position(0) = 1.2 * sin(angle * M_PI / 180);
    ++angle;
    std::cout << _realtimevessel_first.Position << std::endl;
  }
  // get the current position and velocity of the second vessel (K class-II)
  void getcurrentmotion_second(realtimevessel_second &_realtimevessel_second) {
    _realtimevessel_second.Position.setRandom();
    _realtimevessel_second.Velocity.setRandom();
    _realtimevessel_second.Measurement.setRandom();
  }
  // get the current position and velocity of the first vessel (X class)
  void getcurrentmotion_third(realtimevessel_third &_realtimevessel_third) {
    _realtimevessel_third.Position.setRandom();
    _realtimevessel_third.Velocity.setRandom();
    _realtimevessel_third.Measurement.setRandom();
  }
};

#endif /*_MOTIONCAPTURE_H*/
