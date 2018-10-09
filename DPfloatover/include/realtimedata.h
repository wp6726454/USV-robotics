/*
***********************************************************************
* realtimedata.h:
* header file to define the realtime parameters, dependent on each
* autonomous system.
* This header file can be read by both C and C++ compilers
*
*  by Hu.ZH(Mr. SJTU)
***********************************************************************
*/

#ifndef _REALTIMEDATA_H_
#define _REALTIMEDATA_H_
#include <Eigen/Core>

typedef Eigen::Matrix<double, 6, 6> Matrix66d;
typedef Eigen::Matrix<double, 6, 3> Matrix63d;
typedef Eigen::Matrix<double, 6, 1> Vector6d;

// real time data of the first vessel (K class-I)
struct realtimevessel_first {
  /* data wroten by motion capture system */
  // x(surge), y(sway), yaw(theta), u, v, r (next time stamp)
  Vector6d Measurement;
  Vector6d Position;  // x(surge), y(sway), z(heave), roll, pitch, yaw(theta)
  Vector6d Velocity;

  /* data wroten by controller (e.g. kalman, pid, thruster allocation) */
  Vector6d State;             // x(surge), y(sway), yaw(theta), u, v, r
  Eigen::Vector3d tau;        // << x, y, Mz (desired force)
  Eigen::Vector3d BalphaU;    // << x, y, Mz (estimated force)
  Eigen::Vector3d alpha;      // rad, <<  bow_alpha, left_alpha, right_alpha
  Eigen::Vector3i alpha_deg;  // deg, <<  bow_alpha, left_alpha, right_alpha
  Eigen::Vector3d u;          // << bow_thrust, left_thrust, right_thrust
  Eigen::Vector3i rotation;   // rpm, << bow_n, left_n, right_n
};

// real time data of the second vessel (K class-II)
struct realtimevessel_second {
  /* data wroten by motion capture system */
  // x(surge), y(sway), yaw(theta), u, v, r (next time stamp)
  Vector6d Measurement;
  Vector6d Position;  // x(surge), y(sway), z(heave), roll, pitch, yaw(theta)
  Vector6d Velocity;

  /* data wroten by controller (e.g. kalman, pid, thruster allocation) */
  Vector6d State;             // x(surge), y(sway), yaw(theta), u, v, r
  Eigen::Vector3d tau;        // << x, y, Mz (desired force)
  Eigen::Vector3d BalphaU;    // << x, y, Mz (estimated force)
  Eigen::Vector3d alpha;      // rad, <<  bow_alpha, left_alpha, right_alpha
  Eigen::Vector3i alpha_deg;  // deg, <<  bow_alpha, left_alpha, right_alpha
  Eigen::Vector3d u;          // << bow_thrust, left_thrust, right_thrust
  Eigen::Vector3i rotation;   // rpm, << bow_n, left_n, right_n
};

// real time data of the third vessel (X class)
struct realtimevessel_third {
  /* data wroten by motion capture system */
  // x(surge), y(sway), yaw(theta), u, v, r (next time stamp)
  Vector6d Measurement;
  Vector6d Position;  // x(surge), y(sway), z(heave), roll, pitch, yaw(theta)
  Vector6d Velocity;

  /* data wroten by controller (e.g. kalman, pid, thruster allocation) */
  Vector6d State;             // x(surge), y(sway), yaw(theta), u, v, r
  Eigen::Vector3d tau;        // << x, y, Mz (desired force)
  Eigen::Vector3d BalphaU;    // << x, y, Mz (estimated force)
  Eigen::Vector3d alpha;      // rad, <<  bow_alpha, left_alpha, right_alpha
  Eigen::Vector3i alpha_deg;  // deg, <<  bow_alpha, left_alpha, right_alpha
  Eigen::Vector3d u;          // << bow_thrust, left_thrust, right_thrust
  Eigen::Vector3i rotation;   // rpm, << bow_n, left_n, right_n
};

#endif /* _REALTIMEDATA_H_ */