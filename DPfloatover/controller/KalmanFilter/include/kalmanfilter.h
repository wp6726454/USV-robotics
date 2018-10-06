/*
***********************************************************************
* kalmanfilter.h: observer using kalman filter
* function to simulate the kalman filter based on Eigen
* This header file can be read by C++ compilers
*
*  Kalman Filter Class Definition.
*
*  Matrix Dimension must be:
*  x[k] = A * x[k-1] + B * u[k-1] + w[k-1]
*  z[k] = H * x[k] + v[k]
*  x: n x 1
*  z: m x 1
*  u: l x 1
*  A: n x n
*  B: n x l
*  H: m x n
*  Q: n x n
*  R: m x m
*  I: n x n
*  P: n x n
*  K: n x n
*
*
*  by Hu.ZH(Mr.SJTU)
***********************************************************************
*/

#ifndef _KALMANFILTER_H_
#define _KALMANFILTER_H_

#include <Eigen/Dense>
#include <fstream>
#include <iostream>

class kalmanfilter {
 public:
  explicit kalmanfilter(const Eigen::MatrixXd &_A, const Eigen::MatrixXd &_B,
                        const Eigen::MatrixXd &_H, const Eigen::MatrixXd &_Q,
                        const Eigen::MatrixXd &_R)
      : A(_A),
        B(_B),
        H(_H),
        Q(_Q),
        R(_R),
        n(_A.rows()),
        l(_B.cols()),
        m(_H.rows()),
        In(n, n),
        Im(m, m) {
    In.setIdentity();
    Im.setIdentity();
  }
  kalmanfilter() = delete;
  ~kalmanfilter() {}

  /* Set Initial Value */
  void setInitial(const Eigen::VectorXd &_X0, const Eigen::MatrixXd &_P0) {
    X0 = _X0;
    P0 = _P0;
  }

  // perform kalman filter for one step
  Eigen::VectorXd kalmanonestep(const Eigen::MatrixXd &_A,
                                const Eigen::MatrixXd &_B,
                                const Eigen::VectorXd &former_U,
                                const Eigen::VectorXd &_Z) {
    updatesystem(_A, _B);
    predict(former_U);
    correct(_Z);
    return getX();
  }
  Eigen::VectorXd kalmanonestep(const Eigen::MatrixXd &_A,
                                const Eigen::VectorXd &former_U,
                                const Eigen::VectorXd &_Z) {
    updatesystem(_A);
    predict(former_U);
    correct(_Z);
    return getX();
  }
  Eigen::VectorXd kalmanonestep(const Eigen::VectorXd &former_U,
                                const Eigen::VectorXd &_Z) {
    predict(former_U);
    correct(_Z);
    return getX();
  }

  Eigen::VectorXd getX() const { return X; }

 private:
  /* Fixed Matrix */
  Eigen::MatrixXd A;  // System dynamics matrix
  Eigen::MatrixXd B;  // Control matrix
  Eigen::MatrixXd H;  // Mesaurement Adaptation matrix
  Eigen::MatrixXd Q;  // Process Noise Covariance matrix
  Eigen::MatrixXd R;  // Measurement Noise Covariance matrix

  /* Problem Dimension */
  int n;  // State vector dimension
  int l;  // Control vector (input) dimension (if there is not input, set to
          // zero)
  int m;  // observer vector
  Eigen::MatrixXd In;  // Identity matrix
  Eigen::MatrixXd Im;  // Identity matrix
  /* Variable Matrix */
  Eigen::VectorXd X;  //(Current) State vector
  Eigen::MatrixXd P;  // State Covariance
  Eigen::MatrixXd K;  // Kalman Gain matrix

  /* Inizial Value */
  Eigen::VectorXd X0;  // Initial State vector
  Eigen::MatrixXd P0;  // Initial State Covariance matrix

  /* Do prediction based of physical system (No external input) */
  void predict(void) {
    X = A * X0;
    P = A * P0 * A.transpose() + Q;
  }

  /* Do prediction based of physical system (with external input)
   * U: Control vector
   */
  void predict(const Eigen::VectorXd &U) {
    X = A * X0 + B * U;
    P = A * P0 * A.transpose() + Q;
  }

  /* Correct the prediction, using mesaurement
   *  Z: mesaure vector
   */
  void correct(const Eigen::VectorXd &Z) {
    // K = (P * H.transpose()) * (H * P * H.transpose() + R).inverse();
    K = (P * H.transpose()) * (H * P * H.transpose() + R).llt().solve(Im);
    X = X + K * (Z - H * X);

    P = (In - K * H) * P;

    X0 = X;
    P0 = P;
  }

  /*Set Fixed Matrix(NO INPUT) */
  void updatesystem(const Eigen::MatrixXd &_A, const Eigen::MatrixXd &_B) {
    A = _A;
    B = _B;
  }
  /*Set Fixed Matrix(NO INPUT) */
  void updatesystem(const Eigen::MatrixXd &_A) { A = _A; }
};

#endif /* _KALMANFILTER_H_ */