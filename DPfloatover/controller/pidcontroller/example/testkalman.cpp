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
  vessel_first myvessel_first;
  realtimevessel_first myrealvessel;
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
  kalmanfilter_first filter2(myvessel_first, myrealvessel);
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
