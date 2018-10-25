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
  explicit controller_first(const vessel_first &_vessel,
                            realtimevessel_first &_realtimedata)
      : mykalmanfilter(_vessel),
        mypidcontroller(_vessel),
        mythrusterallocation(_vessel, _realtimedata) {
    initializeController(_vessel, _realtimedata);
  }
  controller_first() = delete;
  ~controller_first() {}
  // automatic control using kalman, pid controller, and QP thruster allocation
  void pidcontrolleronestep(realtimevessel_first &_realtimedata, FILE *t_file) {
    mykalmanfilter.kalmanonestep(_realtimedata);
    mypidcontroller.calculategeneralizeforce(_realtimedata);
    mythrusterallocation.onestepthrusterallocation(_realtimedata, t_file);
  }
  void pidcontrolleronestep(realtimevessel_first &_realtimedata,
                            const Eigen::Vector3d &_setpoints, FILE *t_file) {
    mykalmanfilter.kalmanonestep(_realtimedata, _setpoints(2));
    mypidcontroller.calculategeneralizeforce(_realtimedata, _setpoints);
    mythrusterallocation.onestepthrusterallocation(_realtimedata, t_file);
  }
  // automatic heading control, and manual control in x, y direction
  void headingcontrolleronestep(realtimevessel_first &_realtimedata, int xforce,
                                int yforce, FILE *t_file) {
    mykalmanfilter.kalmanonestep(_realtimedata);
    mypidcontroller.calculategeneralizeforce(_realtimedata);
    setGeneralizeForce(_realtimedata, xforce, yforce);
    mythrusterallocation.onestepthrusterallocation(_realtimedata, t_file);
  }
  void headingcontrolleronestep(realtimevessel_first &_realtimedata,
                                const Eigen::Vector3d &_setpoints, int xforce,
                                int yforce, FILE *t_file) {
    mykalmanfilter.kalmanonestep(_realtimedata, _setpoints(2));
    mypidcontroller.calculategeneralizeforce(_realtimedata, _setpoints);
    setGeneralizeForce(_realtimedata, xforce, yforce);
    mythrusterallocation.onestepthrusterallocation(_realtimedata, t_file);
  }

  // manual control in x,y and Mz direction
  void fullymanualcontroller(int xforce, int yforce, int zmoment,
                             realtimevessel_first &_realtimedata,
                             FILE *t_file) {
    setGeneralizeForce(_realtimedata, xforce, yforce, zmoment);
    mythrusterallocation.onestepthrusterallocation(_realtimedata, t_file);
  }

 private:
  kalmanfilter_first mykalmanfilter;              // kalman filter
  pidcontroller_first mypidcontroller;            // pid controller
  thrusterallocation_first mythrusterallocation;  // thruster allocation

  double maxpositive_x_thruster;
  double maxnegative_x_thruster;
  double maxpositive_y_thruster;
  double maxnegative_y_thruster;
  double maxpositive_Mz_thruster;
  double maxnegative_Mz_thruster;
  // initialize the controller
  void initializeController(const vessel_first &_vessel,
                            const realtimevessel_first &_realtimedata) {
    maxpositive_x_thruster = _vessel.maxpositive_x_thruster;
    maxnegative_x_thruster = _vessel.maxnegative_x_thruster;
    maxpositive_y_thruster = _vessel.maxpositive_y_thruster;
    maxnegative_y_thruster = _vessel.maxnegative_y_thruster;
    maxpositive_Mz_thruster = _vessel.maxpositive_Mz_thruster;
    maxnegative_Mz_thruster = _vessel.maxnegative_Mz_thruster;
  }

  // specify all the generalize force using gamepad
  void setGeneralizeForce(realtimevessel_first &_realtimedata, int xforce,
                          int yforce, int zmoment) {
    // specify force in x direction
    if (xforce > 0)
      _realtimedata.tau(0) = xforce * maxpositive_x_thruster;
    else
      _realtimedata.tau(0) = xforce * maxnegative_x_thruster;

    // specify force in y direction
    if (yforce > 0)
      _realtimedata.tau(1) = yforce * maxpositive_y_thruster;
    else
      _realtimedata.tau(1) = yforce * maxnegative_y_thruster;

    // specify moment in z direction
    if (zmoment > 0)
      _realtimedata.tau(2) = zmoment * maxpositive_Mz_thruster;
    else
      _realtimedata.tau(2) = zmoment * maxnegative_Mz_thruster;
  }

  // specify all x and y force using gamepad
  void setGeneralizeForce(realtimevessel_first &_realtimedata, int xforce,
                          int yforce) {
    // specify force in x direction
    if (xforce > 0)
      _realtimedata.tau(0) = xforce * maxpositive_x_thruster;
    else
      _realtimedata.tau(0) = xforce * maxnegative_x_thruster;

    // specify force in y direction
    if (yforce > 0)
      _realtimedata.tau(1) = yforce * maxpositive_y_thruster;
    else
      _realtimedata.tau(1) = yforce * maxnegative_y_thruster;
  }

  // update setpoints
  void updatesetpoints();
};

class controller_second {
 public:
  explicit controller_second(const vessel_second &_vessel,
                             realtimevessel_second &_realtimedata)
      : mykalmanfilter(_vessel),
        mypidcontroller(_vessel),
        mythrusterallocation(_vessel, _realtimedata) {
    initializeController(_vessel, _realtimedata);
  }
  controller_second() = delete;
  ~controller_second() {}

  // automatic control using kalman, pid controller, and QP thruster allocation
  void pidcontrolleronestep(realtimevessel_second &_realtimedata,
                            FILE *t_file) {
    mykalmanfilter.kalmanonestep(_realtimedata);
    mypidcontroller.calculategeneralizeforce(_realtimedata);
    mythrusterallocation.onestepthrusterallocation(_realtimedata, t_file);
  }
  void pidcontrolleronestep(realtimevessel_second &_realtimedata,
                            const Eigen::Vector3d &_setpoints, FILE *t_file) {
    mykalmanfilter.kalmanonestep(_realtimedata, _setpoints(2));
    mypidcontroller.calculategeneralizeforce(_realtimedata, _setpoints);
    mythrusterallocation.onestepthrusterallocation(_realtimedata, t_file);
  }
  // automatic control using kalman, pid controller, and QP thruster allocation

  // automatic heading control, and manual control in x, y direction
  void headingcontrolleronestep(realtimevessel_second &_realtimedata,
                                int xforce, int yforce, FILE *t_file) {
    mykalmanfilter.kalmanonestep(_realtimedata);
    mypidcontroller.calculategeneralizeforce(_realtimedata);
    setGeneralizeForce(_realtimedata, xforce, yforce);
    mythrusterallocation.onestepthrusterallocation(_realtimedata, t_file);
  }
  void headingcontrolleronestep(realtimevessel_second &_realtimedata,
                                const Eigen::Vector3d &_setpoints, int xforce,
                                int yforce, FILE *t_file) {
    mykalmanfilter.kalmanonestep(_realtimedata, _setpoints(2));
    mypidcontroller.calculategeneralizeforce(_realtimedata, _setpoints);
    setGeneralizeForce(_realtimedata, xforce, yforce);
    mythrusterallocation.onestepthrusterallocation(_realtimedata, t_file);
  }
  // manual control in x,y and Mz direction
  void fullymanualcontroller(int xforce, int yforce, int zmoment,
                             realtimevessel_second &_realtimedata,
                             FILE *t_file) {
    setGeneralizeForce(_realtimedata, xforce, yforce, zmoment);
    mythrusterallocation.onestepthrusterallocation(_realtimedata, t_file);
  }

 private:
  kalmanfilter_second mykalmanfilter;              // kalman filter
  pidcontroller_second mypidcontroller;            // pid controller
  thrusterallocation_second mythrusterallocation;  // thruster allocation

  double maxpositive_x_thruster;
  double maxnegative_x_thruster;
  double maxpositive_y_thruster;
  double maxnegative_y_thruster;
  double maxpositive_Mz_thruster;
  double maxnegative_Mz_thruster;

  // initialize the controller
  void initializeController(const vessel_second &_vessel,
                            const realtimevessel_second &_realtimedata) {
    maxpositive_x_thruster = _vessel.maxpositive_x_thruster;
    maxnegative_x_thruster = _vessel.maxnegative_x_thruster;
    maxpositive_y_thruster = _vessel.maxpositive_y_thruster;
    maxnegative_y_thruster = _vessel.maxnegative_y_thruster;
    maxpositive_Mz_thruster = _vessel.maxpositive_Mz_thruster;
    maxnegative_Mz_thruster = _vessel.maxnegative_Mz_thruster;
  }

  // specify all the generalize force using gamepad
  void setGeneralizeForce(realtimevessel_second &_realtimedata, int xforce,
                          int yforce, int zmoment) {
    // specify force in x direction
    if (xforce > 0)
      _realtimedata.tau(0) = xforce * maxpositive_x_thruster;
    else
      _realtimedata.tau(0) = xforce * maxnegative_x_thruster;

    // specify force in y direction
    if (yforce > 0)
      _realtimedata.tau(1) = yforce * maxpositive_y_thruster;
    else
      _realtimedata.tau(1) = yforce * maxnegative_y_thruster;

    // specify moment in z direction
    if (zmoment > 0)
      _realtimedata.tau(2) = zmoment * maxpositive_Mz_thruster;
    else
      _realtimedata.tau(2) = zmoment * maxnegative_Mz_thruster;
  }

  // specify all x and y force using gamepad
  void setGeneralizeForce(realtimevessel_second &_realtimedata, int xforce,
                          int yforce) {
    // specify force in x direction
    if (xforce > 0)
      _realtimedata.tau(0) = xforce * maxpositive_x_thruster;
    else
      _realtimedata.tau(0) = xforce * maxnegative_x_thruster;

    // specify force in y direction
    if (yforce > 0)
      _realtimedata.tau(1) = yforce * maxpositive_y_thruster;
    else
      _realtimedata.tau(1) = yforce * maxnegative_y_thruster;
  }
};

class controller_third {
 public:
  explicit controller_third(const vessel_third &_vessel,
                            realtimevessel_third &_realtimedata)
      : mykalmanfilter(_vessel),
        mypidcontroller(_vessel),
        mythrusterallocation(_vessel, _realtimedata) {
    initializeController(_vessel, _realtimedata);
  }
  controller_third() = delete;
  ~controller_third() {}
  // automatic control using kalman, pid controller, and QP thruster allocation
  void pidcontrolleronestep(realtimevessel_third &_realtimedata) {
    mykalmanfilter.kalmanonestep(_realtimedata);
    mypidcontroller.calculategeneralizeforce(_realtimedata);
    mythrusterallocation.onestepthrusterallocation(_realtimedata);
  }
  void pidcontrolleronestep(realtimevessel_third &_realtimedata,
                            const Eigen::Vector3d &_setpoints) {
    mykalmanfilter.kalmanonestep(_realtimedata, _setpoints(2));
    mypidcontroller.calculategeneralizeforce(_realtimedata, _setpoints);
    mythrusterallocation.onestepthrusterallocation(_realtimedata);
  }
  // automatic heading control, and manual control in x, y direction
  void headingcontrolleronestep(realtimevessel_third &_realtimedata, int xforce,
                                int yforce) {
    mykalmanfilter.kalmanonestep(_realtimedata);
    mypidcontroller.calculategeneralizeforce(_realtimedata);
    setGeneralizeForce(_realtimedata, xforce, yforce);
    mythrusterallocation.onestepthrusterallocation(_realtimedata);
  }
  void headingcontrolleronestep(realtimevessel_third &_realtimedata,
                                const Eigen::Vector3d &_setpoints, int xforce,
                                int yforce) {
    mykalmanfilter.kalmanonestep(_realtimedata, _setpoints(2));
    mypidcontroller.calculategeneralizeforce(_realtimedata, _setpoints);
    setGeneralizeForce(_realtimedata, xforce, yforce);
    mythrusterallocation.onestepthrusterallocation(_realtimedata);
  }
  // manual control in x,y and Mz direction
  void fullymanualcontroller(int xforce, int yforce, int zmoment,
                             realtimevessel_third &_realtimedata) {
    setGeneralizeForce(_realtimedata, xforce, yforce, zmoment);
    mythrusterallocation.onestepthrusterallocation(_realtimedata);
  }

 private:
  kalmanfilter_third mykalmanfilter;              // kalman filter
  pidcontroller_third mypidcontroller;            // pid controller
  thrusterallocation_third mythrusterallocation;  // thruster allocation

  double maxpositive_x_thruster;
  double maxnegative_x_thruster;
  double maxpositive_y_thruster;
  double maxnegative_y_thruster;
  double maxpositive_Mz_thruster;
  double maxnegative_Mz_thruster;

  void initializeController(const vessel_third &_vessel,
                            const realtimevessel_third &_realtimedata) {
    maxpositive_x_thruster = _vessel.maxpositive_x_thruster;
    maxnegative_x_thruster = _vessel.maxnegative_x_thruster;
    maxpositive_y_thruster = _vessel.maxpositive_y_thruster;
    maxnegative_y_thruster = _vessel.maxnegative_y_thruster;
    maxpositive_Mz_thruster = _vessel.maxpositive_Mz_thruster;
    maxnegative_Mz_thruster = _vessel.maxnegative_Mz_thruster;
  }

  // specify all the generalize force using gamepad
  void setGeneralizeForce(realtimevessel_third &_realtimedata, int xforce,
                          int yforce, int zmoment) {
    // specify force in x direction
    if (xforce > 0)
      _realtimedata.tau(0) = xforce * maxpositive_x_thruster;
    else
      _realtimedata.tau(0) = xforce * maxnegative_x_thruster;

    // specify force in y direction
    if (yforce > 0)
      _realtimedata.tau(1) = yforce * maxpositive_y_thruster;
    else
      _realtimedata.tau(1) = yforce * maxnegative_y_thruster;

    // specify moment in z direction
    if (zmoment > 0)
      _realtimedata.tau(2) = zmoment * maxpositive_Mz_thruster;
    else
      _realtimedata.tau(2) = zmoment * maxnegative_Mz_thruster;
  }

  // specify all x and y force using gamepad
  void setGeneralizeForce(realtimevessel_third &_realtimedata, int xforce,
                          int yforce) {
    // specify force in x direction
    if (xforce > 0)
      _realtimedata.tau(0) = xforce * maxpositive_x_thruster;
    else
      _realtimedata.tau(0) = xforce * maxnegative_x_thruster;

    // specify force in y direction
    if (yforce > 0)
      _realtimedata.tau(1) = yforce * maxpositive_y_thruster;
    else
      _realtimedata.tau(1) = yforce * maxnegative_y_thruster;
  }
};
#endif /* _CONTROLLER_H_ */