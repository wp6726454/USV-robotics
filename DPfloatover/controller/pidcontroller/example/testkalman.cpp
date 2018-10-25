#include "eigenmvnd.hpp"
#include "kalmanfilter.h"
#include "thrusterallocation.h"

Eigen::Matrix3d CT;
Matrix66d A;  // System dynamics matrix
Matrix63d B;  // Control matrix
Matrix66d H;  // Mesaurement Adaptation matrix
Matrix66d Q;
Matrix66d R;

// calculate the real time coordinate transform matrix
void calculateCoordinateTransform(const realtimevessel_first &_realtimedata) {
  double orientation = _realtimedata.State(2);
  double cvalue = std::cos(orientation);
  double svalue = std::sin(orientation);
  CT(0, 0) = cvalue;
  CT(1, 1) = cvalue;
  CT(0, 1) = -svalue;
  CT(1, 0) = svalue;
}
// initialize parameters in Kalman filter
void initializeAB(const vessel_first &_vessel_first,
                  const realtimevessel_first &_realtimedata) {
  // copy the constant data
  Eigen::Matrix3d Mass(_vessel_first.Mass);
  Eigen::Matrix3d Damping(_vessel_first.Damping);

  // initialize coordinate transformation matrix
  CT.setIdentity();
  calculateCoordinateTransform(_realtimedata);
  // calcualte the A and B in continous equation
  Matrix63d Bk = Matrix63d::Zero();
  Matrix66d Ak = Matrix66d::Zero();
  Eigen::Matrix3d Inv_Mass = Mass.inverse();
  Ak.topRightCorner(3, 3) = CT;
  Ak.bottomRightCorner(3, 3) = -Inv_Mass * Damping;
  Bk.bottomRows(3) = Inv_Mass;

  // calculate discrete time A, B, and H
  A = Matrix66d::Identity() + sample_time * Ak;
  B = sample_time * Bk;
  H = Matrix66d::Identity();
  // specify Q, R, P
  Q = 0.01 * Matrix66d::Identity();
  R = 0.1 * Matrix66d::Identity();
}
// real time update the Kalman filter matrix
void updateKalmanA(const realtimevessel_first &_realtimedata) {
  calculateCoordinateTransform(_realtimedata);
  A.topRightCorner(3, 3) = sample_time * CT;
}

void test3d() {
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
  realtimevessel_first myrealvessel{
      Vector6d::Zero(),         // measurement
      Vector6d::Zero(),         // position
      Vector6d::Zero(),         // velocity
      Vector6d::Zero(),         // state
      Eigen::Vector3d::Zero(),  // tau
      Eigen::Vector3d::Zero(),  // BalphaU
      (Eigen::Vector3d() << -M_PI / 2, M_PI / 30, -M_PI / 30)
          .finished(),                                   // alpha
      Eigen::Vector3i::Zero(),                           // alpha_deg
      (Eigen::Vector3d() << 0.01, 0.2, 0.2).finished(),  // u
      Eigen::Vector3i::Zero()                            // rotation
  };
  Vector6d initx;
  initx << 10, 10, 0.0, 1, 2, 0.1;
  myrealvessel.State = initx;
  initializeAB(myvessel_first, myrealvessel);
  // generate multivariate normal distribution to simulate gaussian noise
  int _TotalT = 50;
  eigenmvnd normal_Q(Eigen::MatrixXd::Zero(6, 1), Q, _TotalT);
  eigenmvnd normal_R(Eigen::MatrixXd::Zero(6, 1), R, _TotalT);
  Eigen::MatrixXd sample_normal_Q = normal_Q.perform_mvnd().get_mvnd_matrix();
  Eigen::MatrixXd sample_normal_R = normal_R.perform_mvnd().get_mvnd_matrix();

  // generate x and x with noise
  Eigen::MatrixXd x_noise(6, _TotalT);
  Eigen::MatrixXd u(3, _TotalT);
  Eigen::MatrixXd z(6, _TotalT);
  z.col(0) = Eigen::MatrixXd::Zero(6, 1);
  u.setZero();
  x_noise.col(0) = myrealvessel.State;
  for (int i = 0; i != (_TotalT - 1); ++i) {
    myrealvessel.State =
        A * myrealvessel.State + B * u.col(i) + sample_normal_Q.col(i);
    z.col(i + 1) = H * myrealvessel.State + sample_normal_R.col(i + 1);
    updateKalmanA(myrealvessel);
    x_noise.col(i + 1) = myrealvessel.State;
  }

  Eigen::MatrixXd save_x = Eigen::MatrixXd::Zero(6, _TotalT);

  save_x.col(0) = initx;
  /* Create The Filter */
  myrealvessel.State = initx;
  kalmanfilter_first filter2(myvessel_first);
  for (int i = 0; i != (_TotalT - 1); ++i) {
    // filter2.kalmanonestep(myrealvessel, z.col(i + 1));
    myrealvessel.Measurement = z.col(i + 1);
    filter2.kalmanonestep(myrealvessel);
    save_x.col(i + 1) = myrealvessel.State;
  }
  write2csvfile("truex.csv", x_noise);
  write2csvfile("observed.csv", z);
  write2csvfile("kalman.csv", save_x);
}

int main() {
  test3d();
  return 0;
}
