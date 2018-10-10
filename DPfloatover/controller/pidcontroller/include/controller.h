/*
***********************************************************************
* controller.h:
* function for controller, including Kalman filter, pid controller,
* and thruster allocation
* This header file can be read by C++ compilers
*
* by Hu.ZH(Mr.SJTU)
***********************************************************************
*/

#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include "constants.h"
#include "kalmanfilter.h"
#include "pidcontroller.h"
#include "realtimedata.h"
#include "thrusterallocation.h"

class controller_first {
 public:
  explicit controller_first(const vessel_first &_vessel_first,
                            realtimevessel_first &_realtimedata)
      : mykalmanfilter(_vessel_first),
        mypidcontroller(_vessel_first),
        mythrusterallocation(_vessel_first, _realtimedata) {
    // initializeController(_vessel_first, _realtimedata);
  }
  controller_first() = delete;
  ~controller_first() {}
  // automatic control using kalman, pid controller, and QP thruster allocation
  void pidcontrolleronestep(const vessel_first &_vessel_first,
                            realtimevessel_first &_realtimedata,
                            const Vector6d &_measuredstate,
                            const Eigen::Vector3d &_setpoints) {
    mykalmanfilter.kalmanonestep(_realtimedata, _measuredstate, _setpoints(2));
    mypidcontroller.calculategeneralizeforce(_realtimedata, _setpoints);
    mythrusterallocation.onestepthrusterallocation(_vessel_first,
                                                   _realtimedata);
  }
  void pidcontrolleronestep(const vessel_first &_vessel_first,
                            realtimevessel_first &_realtimedata,
                            const Eigen::Vector3d &_setpoints) {
    mykalmanfilter.kalmanonestep(_realtimedata, _setpoints(2));
    mypidcontroller.calculategeneralizeforce(_realtimedata, _setpoints);
    mythrusterallocation.onestepthrusterallocation(_vessel_first,
                                                   _realtimedata);
  }
  // automatic heading control, and manual control in x, y direction
  void headingcontrolleronestep(const vessel_first &_vessel_first,
                                realtimevessel_first &_realtimedata,
                                const Vector6d &_measuredstate,
                                const Eigen::Vector3d &_setpoints, int xforce,
                                int yforce) {
    mykalmanfilter.kalmanonestep(_realtimedata, _measuredstate, _setpoints(2));
    mypidcontroller.calculategeneralizeforce(_realtimedata, _setpoints);
    setGeneralizeForce(_vessel_first, _realtimedata, xforce, yforce);
    mythrusterallocation.onestepthrusterallocation(_vessel_first,
                                                   _realtimedata);
  }
  void headingcontrolleronestep(const vessel_first &_vessel_first,
                                realtimevessel_first &_realtimedata,
                                const Eigen::Vector3d &_setpoints, int xforce,
                                int yforce) {
    mykalmanfilter.kalmanonestep(_realtimedata, _setpoints(2));
    mypidcontroller.calculategeneralizeforce(_realtimedata, _setpoints);
    setGeneralizeForce(_vessel_first, _realtimedata, xforce, yforce);
    mythrusterallocation.onestepthrusterallocation(_vessel_first,
                                                   _realtimedata);
  }

  // manual control in x,y and Mz direction
  void fullymanualcontroller(int xforce, int yforce, int zmoment,
                             const vessel_first &_vessel_first,
                             realtimevessel_first &_realtimedata,
                             FILE *t_file) {
    setGeneralizeForce(_vessel_first, _realtimedata, xforce, yforce, zmoment);
    mythrusterallocation.onestepthrusterallocation(_vessel_first, _realtimedata,
                                                   t_file);
  }

 private:
  kalmanfilter_first mykalmanfilter;              // kalman filter
  pidcontroller_first mypidcontroller;            // pid controller
  thrusterallocation_first mythrusterallocation;  // thruster allocation

  // void initializeController(const vessel_first &_vessel_first,
  //                           const realtimevessel_first &_realtimedata) {}

  // specify all the generalize force using gamepad
  void setGeneralizeForce(const vessel_first &_vessel_first,
                          realtimevessel_first &_realtimedata, int xforce,
                          int yforce, int zmoment) {
    // specify force in x direction
    if (xforce > 0)
      _realtimedata.tau(0) = xforce * _vessel_first.maxpositive_x_thruster;
    else
      _realtimedata.tau(0) = xforce * _vessel_first.maxnegative_x_thruster;

    // specify force in y direction
    if (yforce > 0)
      _realtimedata.tau(1) = yforce * _vessel_first.maxpositive_y_thruster;
    else
      _realtimedata.tau(1) = yforce * _vessel_first.maxnegative_y_thruster;

    // specify moment in z direction
    if (zmoment > 0)
      _realtimedata.tau(2) = zmoment * _vessel_first.maxpositive_Mz_thruster;
    else
      _realtimedata.tau(2) = zmoment * _vessel_first.maxnegative_Mz_thruster;
  }

  // specify all x and y force using gamepad
  void setGeneralizeForce(const vessel_first &_vessel_first,
                          realtimevessel_first &_realtimedata, int xforce,
                          int yforce) {
    // specify force in x direction
    if (xforce > 0)
      _realtimedata.tau(0) = xforce * _vessel_first.maxpositive_x_thruster;
    else
      _realtimedata.tau(0) = xforce * _vessel_first.maxnegative_x_thruster;

    // specify force in y direction
    if (yforce > 0)
      _realtimedata.tau(1) = yforce * _vessel_first.maxpositive_y_thruster;
    else
      _realtimedata.tau(1) = yforce * _vessel_first.maxnegative_y_thruster;
  }
};

class controller_second {
 public:
  explicit controller_second(const vessel_second &_vessel,
                             realtimevessel_second &_realtimedata)
      : mykalmanfilter(_vessel),
        mypidcontroller(_vessel),
        mythrusterallocation(_vessel, _realtimedata) {
    // initializeController(_vessel, _realtimedata);
  }
  controller_second() = delete;
  ~controller_second() {}
  // automatic control using kalman, pid controller, and QP thruster allocation
  void pidcontrolleronestep(const vessel_second &_vessel,
                            realtimevessel_second &_realtimedata,
                            const Vector6d &_measuredstate,
                            const Eigen::Vector3d &_setpoints) {
    mykalmanfilter.kalmanonestep(_realtimedata, _measuredstate, _setpoints(2));
    mypidcontroller.calculategeneralizeforce(_realtimedata, _setpoints);
    mythrusterallocation.onestepthrusterallocation(_vessel, _realtimedata);
  }
  void pidcontrolleronestep(const vessel_second &_vessel,
                            realtimevessel_second &_realtimedata,
                            const Eigen::Vector3d &_setpoints) {
    mykalmanfilter.kalmanonestep(_realtimedata, _setpoints(2));
    mypidcontroller.calculategeneralizeforce(_realtimedata, _setpoints);
    mythrusterallocation.onestepthrusterallocation(_vessel, _realtimedata);
  }
  // automatic heading control, and manual control in x, y direction
  void headingcontrolleronestep(const vessel_second &_vessel,
                                realtimevessel_second &_realtimedata,
                                const Vector6d &_measuredstate,
                                const Eigen::Vector3d &_setpoints, int xforce,
                                int yforce) {
    mykalmanfilter.kalmanonestep(_realtimedata, _measuredstate, _setpoints(2));
    mypidcontroller.calculategeneralizeforce(_realtimedata, _setpoints);
    setGeneralizeForce(_vessel, _realtimedata, xforce, yforce);
    mythrusterallocation.onestepthrusterallocation(_vessel, _realtimedata);
  }
  void headingcontrolleronestep(const vessel_second &_vessel,
                                realtimevessel_second &_realtimedata,
                                const Eigen::Vector3d &_setpoints, int xforce,
                                int yforce) {
    mykalmanfilter.kalmanonestep(_realtimedata, _setpoints(2));
    mypidcontroller.calculategeneralizeforce(_realtimedata, _setpoints);
    setGeneralizeForce(_vessel, _realtimedata, xforce, yforce);
    mythrusterallocation.onestepthrusterallocation(_vessel, _realtimedata);
  }
  // manual control in x,y and Mz direction
  void fullymanualcontroller(int xforce, int yforce, int zmoment,
                             const vessel_second &_vessel,
                             realtimevessel_second &_realtimedata,
                             FILE *t_file) {
    setGeneralizeForce(_vessel, _realtimedata, xforce, yforce, zmoment);
    mythrusterallocation.onestepthrusterallocation(_vessel, _realtimedata,
                                                   t_file);
  }

 private:
  kalmanfilter_second mykalmanfilter;              // kalman filter
  pidcontroller_second mypidcontroller;            // pid controller
  thrusterallocation_second mythrusterallocation;  // thruster allocation

  // void initializeController(const vessel_second &_vessel,
  //                           const realtimevessel_second &_realtimedata) {}

  // specify all the generalize force using gamepad
  void setGeneralizeForce(const vessel_second &_vessel,
                          realtimevessel_second &_realtimedata, int xforce,
                          int yforce, int zmoment) {
    // specify force in x direction
    if (xforce > 0)
      _realtimedata.tau(0) = xforce * _vessel.maxpositive_x_thruster;
    else
      _realtimedata.tau(0) = xforce * _vessel.maxnegative_x_thruster;

    // specify force in y direction
    if (yforce > 0)
      _realtimedata.tau(1) = yforce * _vessel.maxpositive_y_thruster;
    else
      _realtimedata.tau(1) = yforce * _vessel.maxnegative_y_thruster;

    // specify moment in z direction
    if (zmoment > 0)
      _realtimedata.tau(2) = zmoment * _vessel.maxpositive_Mz_thruster;
    else
      _realtimedata.tau(2) = zmoment * _vessel.maxnegative_Mz_thruster;
  }

  // specify all x and y force using gamepad
  void setGeneralizeForce(const vessel_second &_vessel,
                          realtimevessel_second &_realtimedata, int xforce,
                          int yforce) {
    // specify force in x direction
    if (xforce > 0)
      _realtimedata.tau(0) = xforce * _vessel.maxpositive_x_thruster;
    else
      _realtimedata.tau(0) = xforce * _vessel.maxnegative_x_thruster;

    // specify force in y direction
    if (yforce > 0)
      _realtimedata.tau(1) = yforce * _vessel.maxpositive_y_thruster;
    else
      _realtimedata.tau(1) = yforce * _vessel.maxnegative_y_thruster;
  }
};

class controller_third {
 public:
  explicit controller_third(const vessel_third &_vessel,
                            realtimevessel_third &_realtimedata)
      : mykalmanfilter(_vessel),
        mypidcontroller(_vessel),
        mythrusterallocation(_vessel, _realtimedata) {
    // initializeController(_vessel, _realtimedata);
  }
  controller_third() = delete;
  ~controller_third() {}
  // automatic control using kalman, pid controller, and QP thruster allocation
  void pidcontrolleronestep(const vessel_third &_vessel,
                            realtimevessel_third &_realtimedata,
                            const Vector6d &_measuredstate,
                            const Eigen::Vector3d &_setpoints) {
    mykalmanfilter.kalmanonestep(_realtimedata, _measuredstate, _setpoints(2));
    mypidcontroller.calculategeneralizeforce(_realtimedata, _setpoints);
    mythrusterallocation.onestepthrusterallocation(_vessel, _realtimedata);
  }
  void pidcontrolleronestep(const vessel_third &_vessel,
                            realtimevessel_third &_realtimedata,
                            const Eigen::Vector3d &_setpoints) {
    mykalmanfilter.kalmanonestep(_realtimedata, _setpoints(2));
    mypidcontroller.calculategeneralizeforce(_realtimedata, _setpoints);
    mythrusterallocation.onestepthrusterallocation(_vessel, _realtimedata);
  }
  // automatic heading control, and manual control in x, y direction
  void headingcontrolleronestep(const vessel_third &_vessel,
                                realtimevessel_third &_realtimedata,
                                const Vector6d &_measuredstate,
                                const Eigen::Vector3d &_setpoints, int xforce,
                                int yforce) {
    mykalmanfilter.kalmanonestep(_realtimedata, _measuredstate, _setpoints(2));
    mypidcontroller.calculategeneralizeforce(_realtimedata, _setpoints);
    setGeneralizeForce(_vessel, _realtimedata, xforce, yforce);
    mythrusterallocation.onestepthrusterallocation(_vessel, _realtimedata);
  }
  void headingcontrolleronestep(const vessel_third &_vessel,
                                realtimevessel_third &_realtimedata,
                                const Eigen::Vector3d &_setpoints, int xforce,
                                int yforce) {
    mykalmanfilter.kalmanonestep(_realtimedata, _setpoints(2));
    mypidcontroller.calculategeneralizeforce(_realtimedata, _setpoints);
    setGeneralizeForce(_vessel, _realtimedata, xforce, yforce);
    mythrusterallocation.onestepthrusterallocation(_vessel, _realtimedata);
  }
  // manual control in x,y and Mz direction
  void fullymanualcontroller(int xforce, int yforce, int zmoment,
                             const vessel_third &_vessel,
                             realtimevessel_third &_realtimedata) {
    setGeneralizeForce(_vessel, _realtimedata, xforce, yforce, zmoment);
    mythrusterallocation.onestepthrusterallocation(_vessel, _realtimedata);
  }

 private:
  kalmanfilter_third mykalmanfilter;              // kalman filter
  pidcontroller_third mypidcontroller;            // pid controller
  thrusterallocation_third mythrusterallocation;  // thruster allocation

  // void initializeController(const vessel_third &_vessel,
  //                           const realtimevessel_third &_realtimedata) {}

  // specify all the generalize force using gamepad
  void setGeneralizeForce(const vessel_third &_vessel,
                          realtimevessel_third &_realtimedata, int xforce,
                          int yforce, int zmoment) {
    // specify force in x direction
    if (xforce > 0)
      _realtimedata.tau(0) = xforce * _vessel.maxpositive_x_thruster;
    else
      _realtimedata.tau(0) = xforce * _vessel.maxnegative_x_thruster;

    // specify force in y direction
    if (yforce > 0)
      _realtimedata.tau(1) = yforce * _vessel.maxpositive_y_thruster;
    else
      _realtimedata.tau(1) = yforce * _vessel.maxnegative_y_thruster;

    // specify moment in z direction
    if (zmoment > 0)
      _realtimedata.tau(2) = zmoment * _vessel.maxpositive_Mz_thruster;
    else
      _realtimedata.tau(2) = zmoment * _vessel.maxnegative_Mz_thruster;
  }

  // specify all x and y force using gamepad
  void setGeneralizeForce(const vessel_third &_vessel,
                          realtimevessel_third &_realtimedata, int xforce,
                          int yforce) {
    // specify force in x direction
    if (xforce > 0)
      _realtimedata.tau(0) = xforce * _vessel.maxpositive_x_thruster;
    else
      _realtimedata.tau(0) = xforce * _vessel.maxnegative_x_thruster;

    // specify force in y direction
    if (yforce > 0)
      _realtimedata.tau(1) = yforce * _vessel.maxpositive_y_thruster;
    else
      _realtimedata.tau(1) = yforce * _vessel.maxnegative_y_thruster;
  }
};
#endif /* _CONTROLLER_H_ */