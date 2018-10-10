/*
***********************************************************************
* pidcontroller.h:
* function for pid controller
* This header file can be read by C++ compilers
*
* by Hu.ZH(Mr.SJTU)
***********************************************************************
*/

#ifndef _PIDCONTROLLER_H_
#define _PIDCONTROLLER_H_

#include <iostream>
#include "constants.h"
#include "realtimedata.h"

// class of pid controller for the first vessel
class pidcontroller_first {
 public:
  explicit pidcontroller_first(const vessel_first &_vessel_first) {
    initializePIDmatrix(_vessel_first);
    initializepidcontroller();
  }
  pidcontroller_first() = delete;
  ~pidcontroller_first() {}

  // calculate the desired force using PID controller
  void calculategeneralizeforce(realtimevessel_first &_realtimedata,
                                const Eigen::Vector3d &_setpoints) {
    setsetpoints(_setpoints);
    // calculate error
    position_error = setpoints - _realtimedata.State.head(3);
    if (compareerror(position_error)) {
      _realtimedata.tau.setZero();
    } else {  // proportional term
      Eigen::Vector3d Pout = matrix_P * position_error;
      // integral term
      position_error_integral += sample_time * position_error;
      Eigen::Vector3d Iout = matrix_I * position_error_integral;
      // derivative term
      Eigen::Vector3d Dout = -matrix_D * _realtimedata.State.tail(3);
      // output
      _realtimedata.tau = Pout + Iout + Dout;
    }
  }

 private:
  // constant
  Eigen::Matrix3d matrix_P;
  Eigen::Matrix3d matrix_I;
  Eigen::Matrix3d matrix_D;
  Eigen::Vector3d allowed_error;

  // real time data
  Eigen::Vector3d position_error;
  Eigen::Vector3d position_error_integral;
  Eigen::Vector3d setpoints;
  // initialize the PID matrix
  void initializePIDmatrix(const vessel_first &_vessel_first) {
    // pid matrix
    matrix_P.setZero();
    matrix_I.setZero();
    matrix_D.setZero();
    matrix_P(0, 0) = _vessel_first.P_x;
    matrix_P(1, 1) = _vessel_first.P_y;
    matrix_P(2, 2) = _vessel_first.P_theta;
    matrix_I(0, 0) = _vessel_first.I_x;
    matrix_I(1, 1) = _vessel_first.I_y;
    matrix_I(2, 2) = _vessel_first.I_theta;
    matrix_D(0, 0) = _vessel_first.D_x;
    matrix_D(1, 1) = _vessel_first.D_y;
    matrix_D(2, 2) = _vessel_first.D_theta;
    // error
    allowed_error(0) = _vessel_first.allowed_error_x;
    allowed_error(1) = _vessel_first.allowed_error_y;
    allowed_error(2) = _vessel_first.allowed_error_orientation;
  }
  void initializepidcontroller() {
    position_error.setZero();
    position_error_integral.setZero();
    setpoints.setZero();
  }
  // specify the real time setpoints
  void setsetpoints(const Eigen::Vector3d &_setpoints) {
    setpoints = _setpoints;
  }
  // compare the real time error with the allowed error
  bool compareerror(const Eigen::Vector3d &_realtime_error) {
    for (int i = 0; i != 3; ++i)
      if (abs(_realtime_error(i)) > allowed_error(i)) return false;
    return true;
  }
};

// class of pid controller for the second vessel
class pidcontroller_second {
 public:
  explicit pidcontroller_second(const vessel_second &_vessel_second) {
    initializePIDmatrix(_vessel_second);
    initializepidcontroller();
  }
  pidcontroller_second() = delete;
  ~pidcontroller_second() {}

  // calculate the desired force using PID controller
  void calculategeneralizeforce(realtimevessel_second &_realtimedata,
                                const Eigen::Vector3d &_setpoints) {
    setsetpoints(_setpoints);
    // calculate error
    position_error = setpoints - _realtimedata.State.head(3);
    if (compareerror(position_error)) {
      _realtimedata.tau.setZero();
    } else {  // proportional term
      Eigen::Vector3d Pout = matrix_P * position_error;
      // integral term
      position_error_integral += sample_time * position_error;
      Eigen::Vector3d Iout = matrix_I * position_error_integral;
      // derivative term
      Eigen::Vector3d Dout = -matrix_D * _realtimedata.State.tail(3);
      // output
      _realtimedata.tau = Pout + Iout + Dout;
    }
  }

 private:
  // constant
  Eigen::Matrix3d matrix_P;
  Eigen::Matrix3d matrix_I;
  Eigen::Matrix3d matrix_D;
  Eigen::Vector3d allowed_error;
  // real time data
  Eigen::Vector3d position_error;
  Eigen::Vector3d position_error_integral;
  Eigen::Vector3d setpoints;
  // initialize the PID matrix
  void initializePIDmatrix(const vessel_second &_vessel_second) {
    // pid matrix
    matrix_P.setZero();
    matrix_I.setZero();
    matrix_D.setZero();
    matrix_P(0, 0) = _vessel_second.P_x;
    matrix_P(1, 1) = _vessel_second.P_y;
    matrix_P(2, 2) = _vessel_second.P_theta;
    matrix_I(0, 0) = _vessel_second.I_x;
    matrix_I(1, 1) = _vessel_second.I_y;
    matrix_I(2, 2) = _vessel_second.I_theta;
    matrix_D(0, 0) = _vessel_second.D_x;
    matrix_D(1, 1) = _vessel_second.D_y;
    matrix_D(2, 2) = _vessel_second.D_theta;
    // error
    allowed_error(0) = _vessel_second.allowed_error_x;
    allowed_error(1) = _vessel_second.allowed_error_y;
    allowed_error(2) = _vessel_second.allowed_error_orientation;
  }
  void initializepidcontroller() {
    position_error.setZero();
    position_error_integral.setZero();
    setpoints.setZero();
  }
  // specify the real time setpoints
  void setsetpoints(const Eigen::Vector3d &_setpoints) {
    setpoints = _setpoints;
  }
  // compare the real time error with the allowed error
  bool compareerror(const Eigen::Vector3d &_realtime_error) {
    for (int i = 0; i != 3; ++i)
      if (abs(_realtime_error(i)) > allowed_error(i)) return false;
    return true;
  }
};

// class of pid controller for the third vessel
class pidcontroller_third {
 public:
  explicit pidcontroller_third(const vessel_third &_vessel_third) {
    initializePIDmatrix(_vessel_third);
    initializepidcontroller();
  }
  pidcontroller_third() = delete;
  ~pidcontroller_third() {}

  // calculate the desired force using PID controller
  void calculategeneralizeforce(realtimevessel_third &_realtimedata,
                                const Eigen::Vector3d &_setpoints) {
    setsetpoints(_setpoints);
    // calculate error
    position_error = setpoints - _realtimedata.State.head(3);
    if (compareerror(position_error)) {
      _realtimedata.tau.setZero();
    } else {  // proportional term
      Eigen::Vector3d Pout = matrix_P * position_error;
      // integral term
      position_error_integral += sample_time * position_error;
      Eigen::Vector3d Iout = matrix_I * position_error_integral;
      // derivative term
      Eigen::Vector3d Dout = -matrix_D * _realtimedata.State.tail(3);
      // output
      _realtimedata.tau = Pout + Iout + Dout;
    }
  }

 private:
  // constant
  Eigen::Matrix3d matrix_P;
  Eigen::Matrix3d matrix_I;
  Eigen::Matrix3d matrix_D;
  Eigen::Vector3d allowed_error;
  // real time data
  Eigen::Vector3d position_error;
  Eigen::Vector3d position_error_integral;
  Eigen::Vector3d setpoints;
  // initialize the PID matrix
  void initializePIDmatrix(const vessel_third &_vessel_third) {
    // pid matrix
    matrix_P.setZero();
    matrix_I.setZero();
    matrix_D.setZero();
    matrix_P(0, 0) = _vessel_third.P_x;
    matrix_P(1, 1) = _vessel_third.P_y;
    matrix_P(2, 2) = _vessel_third.P_theta;
    matrix_I(0, 0) = _vessel_third.I_x;
    matrix_I(1, 1) = _vessel_third.I_y;
    matrix_I(2, 2) = _vessel_third.I_theta;
    matrix_D(0, 0) = _vessel_third.D_x;
    matrix_D(1, 1) = _vessel_third.D_y;
    matrix_D(2, 2) = _vessel_third.D_theta;
    // error
    allowed_error(0) = _vessel_third.allowed_error_x;
    allowed_error(1) = _vessel_third.allowed_error_y;
    allowed_error(2) = _vessel_third.allowed_error_orientation;
  }
  void initializepidcontroller() {
    position_error.setZero();
    position_error_integral.setZero();
    setpoints.setZero();
  }
  // specify the real time setpoints
  void setsetpoints(const Eigen::Vector3d &_setpoints) {
    setpoints = _setpoints;
  }
  // compare the real time error with the allowed error
  bool compareerror(const Eigen::Vector3d &_realtime_error) {
    for (int i = 0; i != 3; ++i)
      if (abs(_realtime_error(i)) > allowed_error(i)) return false;
    return true;
  }
};
#endif /* _PIDCONTROLLER_H_ */