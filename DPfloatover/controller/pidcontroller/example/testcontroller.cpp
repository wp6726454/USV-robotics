#include <iostream>
#include "controller.h"
#include "eigenmvnd.hpp"

Eigen::Matrix3d CT_first;
Matrix66d A_first;  // System dynamics matrix
Matrix63d B_first;  // Control matrix
Matrix66d H_first;  // Mesaurement Adaptation matrix
Matrix66d Q_first;
Matrix66d R_first;

// calculate the real time coordinate transform matrix
void calculateCoordinateTransform(double orientation) {
  double cvalue = std::cos(orientation);
  double svalue = std::sin(orientation);
  CT_first(0, 0) = cvalue;
  CT_first(1, 1) = cvalue;
  CT_first(0, 1) = -svalue;
  CT_first(1, 0) = svalue;
}

// initialize parameters in Kalman filter
void initializeAB(const vessel_first &_vessel_first) {
  // copy the constant data
  Eigen::Matrix3d Mass(_vessel_first.Mass);
  Eigen::Matrix3d Damping(_vessel_first.Damping);

  // initialize coordinate transformation matrix
  CT_first.setIdentity();
  // calcualte the A and B in continous equation
  Matrix63d Bk = Matrix63d::Zero();
  Matrix66d Ak = Matrix66d::Zero();
  Eigen::Matrix3d Inv_Mass = Mass.inverse();
  Ak.topRightCorner(3, 3) = CT_first;
  Ak.bottomRightCorner(3, 3) = -Inv_Mass * Damping;
  Bk.bottomRows(3) = Inv_Mass;

  // calculate discrete time A, B, and H
  A_first = Matrix66d::Identity() + sample_time * Ak;
  B_first = sample_time * Bk;
  H_first = Matrix66d::Identity();
  // specify Q, R, P
  Q_first = 0.01 * Matrix66d::Identity();
  R_first = 0.1 * Matrix66d::Identity();
}

// real time update the Kalman filter matrix
void updateKalmanA_first(double orientation) {
  calculateCoordinateTransform(orientation);
  A_first.topRightCorner(3, 3) = sample_time * CT_first;
}

Eigen::Matrix3d CT_second;
Matrix66d A_second;  // System dynamics matrix
Matrix63d B_second;  // Control matrix
Matrix66d H_second;  // Mesaurement Adaptation matrix
Matrix66d Q_second;
Matrix66d R_second;

// initialize parameters in Kalman filter
void initializeAB(const vessel_second &_vessel_second) {
  // copy the constant data
  Eigen::Matrix3d Mass(_vessel_second.Mass);
  Eigen::Matrix3d Damping(_vessel_second.Damping);

  // initialize coordinate transformation matrix
  CT_second.setIdentity();
  // calcualte the A and B in continous equation
  Matrix63d Bk = Matrix63d::Zero();
  Matrix66d Ak = Matrix66d::Zero();
  Eigen::Matrix3d Inv_Mass = Mass.inverse();
  Ak.topRightCorner(3, 3) = CT_second;
  Ak.bottomRightCorner(3, 3) = -Inv_Mass * Damping;
  Bk.bottomRows(3) = Inv_Mass;

  // calculate discrete time A, B, and H
  A_second = Matrix66d::Identity() + sample_time * Ak;
  B_second = sample_time * Bk;
  H_second = Matrix66d::Identity();
  // specify Q, R, P
  Q_second = 0.01 * Matrix66d::Identity();
  R_second = 0.1 * Matrix66d::Identity();
}

// real time update the Kalman filter matrix
void updateKalmanA_second(double orientation) {
  calculateCoordinateTransform(orientation);
  A_second.topRightCorner(3, 3) = sample_time * CT_second;
}

Eigen::Matrix3d CT_third;
Matrix66d A_third;  // System dynamics matrix
Matrix63d B_third;  // Control matrix
Matrix66d H_third;  // Mesaurement Adaptation matrix
Matrix66d Q_third;
Matrix66d R_third;

// initialize parameters in Kalman filter
void initializeAB(const vessel_third &_vessel_third) {
  // copy the constant data
  Eigen::Matrix3d Mass(_vessel_third.Mass);
  Eigen::Matrix3d Damping(_vessel_third.Damping);

  // initialize coordinate transformation matrix
  CT_third.setIdentity();
  // calcualte the A and B in continous equation
  Matrix63d Bk = Matrix63d::Zero();
  Matrix66d Ak = Matrix66d::Zero();
  Eigen::Matrix3d Inv_Mass = Mass.inverse();
  Ak.topRightCorner(3, 3) = CT_third;
  Ak.bottomRightCorner(3, 3) = -Inv_Mass * Damping;
  Bk.bottomRows(3) = Inv_Mass;

  // calculate discrete time A, B, and H
  A_third = Matrix66d::Identity() + sample_time * Ak;
  B_third = sample_time * Bk;
  H_third = Matrix66d::Identity();
  // specify Q, R, P
  Q_third = 0.01 * Matrix66d::Identity();
  R_third = 0.1 * Matrix66d::Identity();
}

// real time update the Kalman filter matrix
void updateKalmanA(double orientation) {
  calculateCoordinateTransform(orientation);
  A_third.topRightCorner(3, 3) = sample_time * CT_third;
}

// initi
int main() {
  // The first vessel

  vessel_first myvessel_first{
      {623, 0, 0, 0, 706, 444, 0, 444, 1298},  // mass
      {17, 0, 0, 0, 20, 0, 0, 0, 100},         // damping
      4,                                       // P_x
      1,                                       // P_y
      5,                                       // P_theta
      0.2,                                     // I_x
      0.1,                                     // I_y
      1,                                       // I_theta
      0.1,                                     // D_x
      0.1,                                     // D_y
      0.2,                                     // D_theta
      0.01,                                    // allowed_error_x
      0.01,                                    // allowed_error_y
      0.01,                                    // allowed_error_orientation
      26.0,                                    // maxpositive_x_thrust(N)
      25.0,                                    // maxnegative_x_thrust(N)
      6,                                       // maxpositive_y_thrust(N)
      4,                                       // maxnegative_y_thrust(N)
      11,                                      // maxpositive_Mz_thrust(N*m)
      7.6,                                     // maxnegative_Mz_thrust(N*m)
      3,                                       // m
      3,                                       // n
      9,                                       // numvar
      3,                                       // num_constraints
      5.6e-7,                                  // Kbar_positive
      2.2e-7,                                  // Kbar_negative
      100,                                     // max_delta_rotation_bow
      4000,                                    // max_rotation_bow
      8.96,                                    // max_thrust_bow_positive
      3.52,                                    // max_thrust_bow_negative
      2e-5,                                    // K_left
      2e-5,                                    // K_right
      20,                                      // max_delta_rotation_bow
      1000,                                    // max_rotation_azimuth
      20,                                      // max_thrust_azimuth_left
      20,                                      // max_thrust_azimuth_right
      0.1277,                                  // max_delta_alpha_azimuth
      M_PI,                                    // max_alpha_azimuth_left
      0,                                       // min_alpha_azimuth_left
      0,                                       // max_alpha_azimuth_right
      -M_PI,                                   // min_alpha_azimuth_right
      1.9,                                     // bow_x
      0,                                       // bow_y
      -1.893,                                  // left_x
      -0.216,                                  // left_y
      -1.893,                                  // right_x
      0.216                                    // right_y
  };
  realtimevessel_first myrealtimevessel_first;
  myrealtimevessel_first.tau.setZero();
  myrealtimevessel_first.alpha << -M_PI / 2, M_PI / 180, -M_PI / 30;
  myrealtimevessel_first.u << 0.001, 0.0, 0.0;
  myrealtimevessel_first.State << 0, 0.2, M_PI / 18, 0, 0, 0;
  controller_first mycontroller_first(myvessel_first, myrealtimevessel_first);
  initializeAB(myvessel_first);
  // generate multivariate normal distribution to simulate gaussian noise
  Eigen::Matrix3d R_first;
  R_first << 2, 0, 0, 0, 1, 0, 0, 0, 2;
  int _TotalT = 2000;
  eigenmvnd normal_R_first(Eigen::Vector3d::Zero(), R_first, _TotalT);
  Eigen::MatrixXd sample_normal_R_first =
      normal_R_first.perform_mvnd().get_mvnd_matrix();
  Eigen::MatrixXd z_first = Eigen::MatrixXd::Zero(6, _TotalT);
  z_first.col(0) = myrealtimevessel_first.State;

  Eigen::MatrixXd save_x_first = Eigen::MatrixXd::Zero(6, _TotalT);
  Eigen::MatrixXd save_u_first = Eigen::MatrixXd::Zero(3, _TotalT);
  Eigen::MatrixXd save_alpha_first = Eigen::MatrixXd::Zero(3, _TotalT);
  Eigen::MatrixXd save_tau_first = Eigen::MatrixXd::Zero(3, _TotalT);
  Eigen::MatrixXd save_BalphaU_first = Eigen::MatrixXd::Zero(3, _TotalT);
  Eigen::Vector3d setpoints_first;
  setpoints_first << 0, 0, 0;
  save_x_first.col(0) = myrealtimevessel_first.State;
  save_u_first.col(0) = myrealtimevessel_first.u;
  save_alpha_first.col(0) = myrealtimevessel_first.alpha;
  save_tau_first.col(0) = myrealtimevessel_first.tau;
  for (int i = 0; i != (_TotalT - 1); ++i) {
    z_first.col(i + 1) = A_first * myrealtimevessel_first.State +
                         B_first * (sample_normal_R_first.col(i) +
                                    myrealtimevessel_first.BalphaU);
    myrealtimevessel_first.Measurement = z_first.col(i + 1);
    mycontroller_first.pidcontrolleronestep(myrealtimevessel_first,
                                            setpoints_first);
    // updateKalmanA(myrealtimevessel);
    save_x_first.col(i + 1) = myrealtimevessel_first.State;
    save_u_first.col(i + 1) = myrealtimevessel_first.u;
    save_alpha_first.col(i + 1) = myrealtimevessel_first.alpha;
    save_tau_first.col(i + 1) = myrealtimevessel_first.tau;
    save_BalphaU_first.col(i + 1) = myrealtimevessel_first.BalphaU;
  }
  write2csvfile("first_x.csv", save_x_first);
  write2csvfile("first_u.csv", save_u_first);
  write2csvfile("first_alpha.csv", save_alpha_first);
  write2csvfile("first_tau.csv", save_tau_first);
  write2csvfile("first_BalphaU.csv", save_BalphaU_first);

  // The second vessel
  // constant parameters of the second vessel
  vessel_second myvessel_second{
      {623, 0, 0, 0, 706, 444, 0, 444, 1298},  // mass
      {17, 0, 0, 0, 20, 0, 0, 0, 100},         // damping
      4,                                       // P_x
      1,                                       // P_y
      5,                                       // P_theta
      0.2,                                     // I_x
      0.1,                                     // I_y
      1,                                       // I_theta
      0.1,                                     // D_x
      0.1,                                     // D_y
      0.2,                                     // D_theta
      0.01,                                    // allowed_error_x
      0.01,                                    // allowed_error_y
      0.01,                                    // allowed_error_orientation
      26.0,                                    // maxpositive_x_thrust(N)
      25.0,                                    // maxnegative_x_thrust(N)
      6,                                       // maxpositive_y_thrust(N)
      4,                                       // maxnegative_y_thrust(N)
      11,                                      // maxpositive_Mz_thrust(N*m)
      7.6,                                     // maxnegative_Mz_thrust(N*m)
      3,                                       // m
      3,                                       // n
      9,                                       // numvar
      3,                                       // num_constraints
      5.6e-7,                                  // Kbar_positive
      2.2e-7,                                  // Kbar_negative
      100,                                     // max_delta_rotation_bow
      4000,                                    // max_rotation_bow
      8.96,                                    // max_thrust_bow_positive
      3.52,                                    // max_thrust_bow_negative
      2e-5,                                    // K_left
      2e-5,                                    // K_right
      20,                                      // max_delta_rotation_bow
      1000,                                    // max_rotation_azimuth
      20,                                      // max_thrust_azimuth_left
      20,                                      // max_thrust_azimuth_right
      0.1277,                                  // max_delta_alpha_azimuth
      M_PI,                                    // max_alpha_azimuth_left
      0,                                       // min_alpha_azimuth_left
      0,                                       // max_alpha_azimuth_right
      -M_PI,                                   // min_alpha_azimuth_right
      1.9,                                     // bow_x
      0,                                       // bow_y
      -1.893,                                  // left_x
      -0.216,                                  // left_y
      -1.893,                                  // right_x
      0.216                                    // right_y
  };

  realtimevessel_second myrealtimevessel_second;
  myrealtimevessel_second.tau.setZero();
  myrealtimevessel_second.alpha << -M_PI / 2, M_PI / 180, -M_PI / 30;
  myrealtimevessel_second.u << 0.001, 0.0, 0.0;
  myrealtimevessel_second.State << 0, 0.2, M_PI / 180, 0, 0, 0;
  controller_second mycontroller_second(myvessel_second,
                                        myrealtimevessel_second);
  initializeAB(myvessel_second);
  // generate multivariate normal distribution to simulate gaussian noise
  Eigen::Matrix3d R_second;
  R_second << 2, 0, 0, 0, 1, 0, 0, 0, 10;
  eigenmvnd normal_R_second(Eigen::Vector3d::Zero(), R_second, _TotalT);
  Eigen::MatrixXd sample_normal_R_second =
      normal_R_second.perform_mvnd().get_mvnd_matrix();
  Eigen::MatrixXd z_second = Eigen::MatrixXd::Zero(6, _TotalT);
  z_second.col(0) = myrealtimevessel_second.State;

  Eigen::MatrixXd save_x_second = Eigen::MatrixXd::Zero(6, _TotalT);
  Eigen::MatrixXd save_u_second = Eigen::MatrixXd::Zero(3, _TotalT);
  Eigen::MatrixXd save_alpha_second = Eigen::MatrixXd::Zero(3, _TotalT);
  Eigen::MatrixXd save_tau_second = Eigen::MatrixXd::Zero(3, _TotalT);
  Eigen::MatrixXd save_BalphaU_second = Eigen::MatrixXd::Zero(3, _TotalT);
  Eigen::Vector3d setpoints_second;
  setpoints_second << 0, 0, 0;
  save_x_second.col(0) = myrealtimevessel_second.State;
  save_u_second.col(0) = myrealtimevessel_second.u;
  save_alpha_second.col(0) = myrealtimevessel_second.alpha;
  save_tau_second.col(0) = myrealtimevessel_second.tau;
  for (int i = 0; i != (_TotalT - 1); ++i) {
    z_second.col(i + 1) = A_second * myrealtimevessel_second.State +
                          B_second * (sample_normal_R_second.col(i) +
                                      myrealtimevessel_second.BalphaU);
    mycontroller_second.pidcontrolleronestep(
        myrealtimevessel_second, z_second.col(i + 1), setpoints_second);
    // updateKalmanA(myrealtimevessel);
    save_x_second.col(i + 1) = myrealtimevessel_second.State;
    save_u_second.col(i + 1) = myrealtimevessel_second.u;
    save_alpha_second.col(i + 1) = myrealtimevessel_second.alpha;
    save_tau_second.col(i + 1) = myrealtimevessel_second.tau;
    save_BalphaU_second.col(i + 1) = myrealtimevessel_second.BalphaU;
  }
  write2csvfile("second_x.csv", save_x_second);
  write2csvfile("second_u.csv", save_u_second);
  write2csvfile("second_alpha.csv", save_alpha_second);
  write2csvfile("second_tau.csv", save_tau_second);
  write2csvfile("second_BalphaU.csv", save_BalphaU_second);

  // The third vessel

  // constant parameters of the third vessel
  vessel_third myvessel_third{
      {623, 0, 0, 0, 706, 444, 0, 444, 1298},  // mass
      {17, 0, 0, 0, 20, 0, 0, 0, 100},         // damping
      4,                                       // P_x
      1,                                       // P_y
      5,                                       // P_theta
      0.2,                                     // I_x
      0.1,                                     // I_y
      1,                                       // I_theta
      0.1,                                     // D_x
      0.1,                                     // D_y
      0.2,                                     // D_theta
      0.01,                                    // allowed_error_x
      0.01,                                    // allowed_error_y
      0.01,                                    // allowed_error_orientation
      26.0,                                    // maxpositive_x_thrust(N)
      25.0,                                    // maxnegative_x_thrust(N)
      6,                                       // maxpositive_y_thrust(N)
      4,                                       // maxnegative_y_thrust(N)
      11,                                      // maxpositive_Mz_thrust(N*m)
      7.6,                                     // maxnegative_Mz_thrust(N*m)
      3,                                       // m
      3,                                       // n
      9,                                       // numvar
      3,                                       // num_constraints
      5.6e-7,                                  // Kbar_positive
      2.2e-7,                                  // Kbar_negative
      100,                                     // max_delta_rotation_bow
      4000,                                    // max_rotation_bow
      8.96,                                    // max_thrust_bow_positive
      3.52,                                    // max_thrust_bow_negative
      2e-5,                                    // K_left
      2e-5,                                    // K_right
      20,                                      // max_delta_rotation_bow
      1000,                                    // max_rotation_azimuth
      20,                                      // max_thrust_azimuth_left
      20,                                      // max_thrust_azimuth_right
      0.1277,                                  // max_delta_alpha_azimuth
      M_PI,                                    // max_alpha_azimuth_left
      0,                                       // min_alpha_azimuth_left
      0,                                       // max_alpha_azimuth_right
      -M_PI,                                   // min_alpha_azimuth_right
      1.9,                                     // bow_x
      0,                                       // bow_y
      -1.893,                                  // left_x
      -0.216,                                  // left_y
      -1.893,                                  // right_x
      0.216                                    // right_y
  };
  realtimevessel_third myrealtimevessel_third;
  myrealtimevessel_third.tau.setZero();
  myrealtimevessel_third.alpha << -M_PI / 2, M_PI / 180, -M_PI / 30;
  myrealtimevessel_third.u << 0.001, 0.0, 0.0;
  myrealtimevessel_third.State << 0, 0.2, M_PI / 180, 0, 0, 0;
  controller_third mycontroller_third(myvessel_third, myrealtimevessel_third);
  initializeAB(myvessel_third);
  // generate multivariate normal distribution to simulate gaussian noise
  Eigen::Matrix3d R_third;
  R_third << 2, 0, 0, 0, 1, 0, 0, 0, 10;
  eigenmvnd normal_R_third(Eigen::Vector3d::Zero(), R_third, _TotalT);
  Eigen::MatrixXd sample_normal_R_third =
      normal_R_third.perform_mvnd().get_mvnd_matrix();
  Eigen::MatrixXd z_third = Eigen::MatrixXd::Zero(6, _TotalT);
  z_third.col(0) = myrealtimevessel_third.State;

  Eigen::MatrixXd save_x_third = Eigen::MatrixXd::Zero(6, _TotalT);
  Eigen::MatrixXd save_u_third = Eigen::MatrixXd::Zero(3, _TotalT);
  Eigen::MatrixXd save_alpha_third = Eigen::MatrixXd::Zero(3, _TotalT);
  Eigen::MatrixXd save_tau_third = Eigen::MatrixXd::Zero(3, _TotalT);
  Eigen::MatrixXd save_BalphaU_third = Eigen::MatrixXd::Zero(3, _TotalT);
  Eigen::Vector3d setpoints_third;
  setpoints_third << 0, 0, 0;
  save_x_third.col(0) = myrealtimevessel_third.State;
  save_u_third.col(0) = myrealtimevessel_third.u;
  save_alpha_third.col(0) = myrealtimevessel_third.alpha;
  save_tau_third.col(0) = myrealtimevessel_third.tau;
  for (int i = 0; i != (_TotalT - 1); ++i) {
    z_third.col(i + 1) = A_third * myrealtimevessel_third.State +
                         B_third * (sample_normal_R_third.col(i) +
                                    myrealtimevessel_third.BalphaU);
    mycontroller_third.pidcontrolleronestep(
        myvessel_third, myrealtimevessel_third, z_third.col(i + 1),
        setpoints_third);
    // updateKalmanA(myrealtimevessel);
    save_x_third.col(i + 1) = myrealtimevessel_third.State;
    save_u_third.col(i + 1) = myrealtimevessel_third.u;
    save_alpha_third.col(i + 1) = myrealtimevessel_third.alpha;
    save_tau_third.col(i + 1) = myrealtimevessel_third.tau;
    save_BalphaU_third.col(i + 1) = myrealtimevessel_third.BalphaU;
  }
  write2csvfile("third_x.csv", save_x_third);
  write2csvfile("third_u.csv", save_u_third);
  write2csvfile("third_alpha.csv", save_alpha_third);
  write2csvfile("third_tau.csv", save_tau_third);
  write2csvfile("third_BalphaU.csv", save_BalphaU_third);
}
