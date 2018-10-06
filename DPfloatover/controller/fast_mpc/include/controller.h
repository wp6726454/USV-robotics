/*
***********************************************************************
* controller.hpp:
* function for controller and optimization (MPC)
* This header file can be read by C++ compilers
*
*  by Hu.ZH(Mr.SJTU)
***********************************************************************
*/
#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include "../../../include/constants.h"
#include "/home/scar1et/Coding/CPP1X/USV/timer/timecounter.hpp"
#include <Eigen/Core>
#include <Eigen/Sparse>
#include <Eigen/SparseCholesky>
#include <fstream>
#include <iostream>
#include <map>
#include <math.h>
#include <stdio.h>
#include <string>
#include <unordered_map>
#include <unsupported/Eigen/MatrixFunctions>
const int m = 3;
const int n = 12;
const int mpn = m + n;
const int T = 30;
const int num_Fx = n;
const int num_Fu = 2 * m;
const int num_Ff = 2 * n;
const int nz = T * mpn;
const int nmu = T * n;

typedef Eigen::Matrix<double, n, n> Matnn;
typedef Eigen::Matrix<double, n, m> Matnm;
typedef Eigen::Matrix<double, m, m> Matmm;
typedef Eigen::Matrix<double, mpn, mpn> Matmpn;
typedef Eigen::Matrix<double, n, 1> Vectn;
typedef Eigen::Matrix<double, m, 1> Vectm;
typedef Eigen::Matrix<double, nz, 1> Vectnz;
typedef Eigen::Matrix<double, nmu, 1> Vectnmu;

class controller {
public:
  controller() {
    initilizeABcts();
    generateABdiscrete(0);
    initilizeMPCparameters();
  }
  ~controller() {}

  void print() {
    // std::cout << Acts << std::endl << Bcts << std::endl;
    // std::cout << A << std::endl << B << std::endl;
    // std::cout << "Q = " << std::endl << Q << std::endl;
    // std::cout << "R = " << std::endl << R << std::endl;
    // std::cout << "S = " << std::endl << S << std::endl;
    // std::cout << "Qf = " << std::endl << Qf << std::endl;
    // std::cout << "q = " << std::endl << q << std::endl;
    // std::cout << "qf = " << std::endl << qf << std::endl;
    // std::cout << "r = " << std::endl << r << std::endl;
    // std::cout << "Fx = " << std::endl << Fx << std::endl;
    // std::cout << "fx = " << std::endl << fx << std::endl;
    // std::cout << "Fu = " << std::endl << Fu << std::endl;
    // std::cout << "fu = " << std::endl << fu << std::endl;
    // std::cout << "Ff = " << std::endl << Ff << std::endl;
    // std::cout << "ff = " << std::endl << ff << std::endl;
    // std::cout << "H = " << std::endl << H << std::endl;
    // std::cout << "g = " << std::endl << g << std::endl;

    // std::cout << "Sparse_H = " << std::endl
    //           << Sparse_H.block(0, 0, 20, 20) << std::endl;
    // std::cout << "Sparse_P = " << std::endl
    //           << Sparse_P.block(0, 0, 20, 20) << std::endl;
    // std::cout << "C = " << std::endl << C.block(0, 0, 20, 20) << std::endl;
    // std::cout << "Sparse_C = " << std::endl
    //           << Sparse_C.block(0, 0, 20, 20) << std::endl;
    // std::cout << Sparse_H.nonZeros() << std::endl;
    // std::cout << Sparse_P.nonZeros() << std::endl;
    // std::cout << Sparse_C.nonZeros() << std::endl;
    std::cout << "delta_mu=" << std::endl << delta_mu << std::endl;
    std::cout << "delta_z=" << std::endl << delta_z << std::endl;
  }
  void output() {
    write2csvfile("u.csv", save_u);
    write2csvfile("x.csv", save_x);
  }

  void basicQP_onestep() {
    timecounter t1("Basic QP");
    Vectm test_u;
    test_u.setZero();
    x0 << 0.0682587, 0.501672, 0.0275302, 0.136795, 0.158625, 0.37932, 0.712373,
        -0.157053, -0.423422, -0.463745, -0.55538, -0.379282;
    basicQP4MPC(x0, test_u, 0.01);
    std::cout << "u = " << test_u << std::endl;
    std::cout << fastcount << std::endl;
  }

  void approximateQP_onestep() {
    timecounter t1("Approximate QP");
    Vectm test_u;
    test_u.setZero();
    x0 << 0.0682587, 0.501672, 0.0275302, 0.136795, 0.158625, 0.37932, 0.712373,
        -0.157053, -0.423422, -0.463745, -0.55538, -0.379282;
    approximateQP4MPC(x0, test_u, 0.1);
    std::cout << "u = " << test_u << std::endl;
    std::cout << fastcount << std::endl;
  }

  void basicQP_multistep() {
    timecounter t1("Multiple step");
    Vectn disturbance;
    disturbance.setZero();
    Vectm temp_u;
    save_x.col(0) = x0;
    for (int i = 0; i != (totalstep - 1); ++i) {
      // solve QP in MPC
      basicQP4MPC(save_x.col(i), temp_u);
      // update x
      disturbance.tail(n / 2) = Eigen::VectorXd::Random(n / 2);
      disturbance.tail(n / 2) = disturbance.tail(n / 2) / 2;
      save_x.col(i + 1) = A * save_x.col(i) + B * temp_u + disturbance;
      save_u.col(i) = temp_u;
    }
  }

  void approximateQP_multistep() {
    timecounter t1("Multiple step");
    Vectn disturbance;
    disturbance.setZero();
    Vectm temp_u;
    save_x.col(0) = x0;
    for (int i = 0; i != (totalstep - 1); ++i) {
      // solve QP in MPC
      approximateQP4MPC(save_x.col(i), temp_u, 0.1);
      // update x
      disturbance.tail(n / 2) = Eigen::VectorXd::Random(n / 2);
      disturbance.tail(n / 2) = disturbance.tail(n / 2) / 2;
      save_x.col(i + 1) = A * save_x.col(i) + B * temp_u + disturbance;
      save_u.col(i) = temp_u;
    }
  }

  void testcomputeresidual() {
    double kappa = 10;
    double totalresidual = computeResidualsAndNorm(z, mu, x0, kappa);
    std::cout << totalresidual << std::endl;
  }

  void compareTwoMethods() {
    // verify the fast computation of Newton Step using schur complement
    FastMPCcomputtion();
    double kappa = 10;
    convertDense2Sparse();
    Vectnz t_delta_z = Vectnz::Zero(nz);
    Vectnmu t_delta_mu = Vectnmu::Zero(nmu);
    SchurcomputeNewtonStep(z, mu, x0, kappa, t_delta_z, t_delta_mu);
    std::cout << "difference norm of z: " << (delta_z - t_delta_z).norm()
              << std::endl;
    std::cout << "difference norm of mu: " << (delta_mu - t_delta_mu).norm()
              << std::endl;
  }

  void SchurComposition() {
    double kappa = 10;
    convertDense2Sparse();
    timecounter t1("Schur");
    SchurcomputeNewtonStep(z, mu, x0, kappa, delta_z, delta_mu);
  }

  void FastMPCcomputtion() {
    double kappa = 10;
    double totalresidualnorm = 0;
    fastcomputeNewtonStep(z, mu, x0, kappa, delta_z, delta_mu,
                          totalresidualnorm);
  }

private:
  int fastcount = 0;

  Matnn Acts;        // A in the continuous-time state space model
  Matnm Bcts;        // B in the continuous-time state space model
  Eigen::MatrixXd A; // A in the discrete-time state space model
  Eigen::MatrixXd B; // B in the discrete-time state space model
  Vectn Omega_bar;   // mean of noise in state space model

  // objective matrices in MPC
  Matnn Q;  // Q in MPC
  Matmm R;  // R in MPC
  Matnm S;  // S in MPC
  Matnn Qf; // Qf in MPC
  Vectn q;  // q in MPC
  Vectn qf; // qf in MPC
  Vectm r;  // r in MPC

  // constraints in MPC
  Eigen::Matrix<double, num_Fx, n> Fx; // Fx in MPC
  Eigen::Matrix<double, num_Fx, 1> fx; // fx in MPC
  Eigen::Matrix<double, num_Fu, m> Fu; // Fu in MPC
  Eigen::Matrix<double, num_Fu, 1> fu; // fu in MPC
  Eigen::Matrix<double, num_Ff, n> Ff; // Ff in MPC
  Eigen::Matrix<double, num_Ff, 1> ff; // ff in MPC

  // Quadratic programming in MPC
  Eigen::MatrixXd H;
  Eigen::MatrixXd P;
  Eigen::MatrixXd C;
  Eigen::VectorXd g;
  Eigen::VectorXd h;
  Eigen::VectorXd b;
  Matmpn Small_H;

  // sparse matrix for QP in MPC
  Eigen::SparseMatrix<double, Eigen::ColMajor> Sparse_H;
  Eigen::SparseMatrix<double, Eigen::ColMajor> Sparse_P;
  Eigen::SparseMatrix<double, Eigen::ColMajor> Sparse_C;

  // real-time data for QP in MPC
  Vectnz z = Vectnz::Zero(nz);
  Vectnmu mu = Vectnmu::Zero(nmu);
  Vectn x0 = Vectn::Zero(n);
  Vectnz delta_z = Vectnz::Zero(nz);
  Vectnmu delta_mu = Vectnmu::Zero(nmu);
  Eigen::VectorXd rd = Eigen::VectorXd::Zero(nz);
  Eigen::VectorXd rp = Eigen::VectorXd::Zero(nmu);
  Eigen::VectorXd d =
      Eigen::VectorXd::Zero(T * num_Fu + (T - 1) * num_Fx + num_Ff);
  Eigen::SparseMatrix<double, Eigen::ColMajor> Sparse_Theta;

  // iteration constant in QP
  double alpha = 0.01;
  double beta = 0.5;
  double tolerance = 1e-3;
  double residuals_tolerance = 0;
  double required_duality_gap = 1e-3;
  double kappa_rate = 0.1;

  // data saved for validation and viewer
  const int totalstep = 1000;
  Eigen::MatrixXd save_x = Eigen::MatrixXd::Zero(n, totalstep);
  Eigen::MatrixXd save_u = Eigen::MatrixXd::Zero(m, totalstep - 1);

  void initilizeABcts() {
    Acts.topLeftCorner(n / 2, n / 2).setZero();
    Acts.topRightCorner(n / 2, n / 2).setIdentity();
    Acts.bottomLeftCorner(n / 2, n / 2) << -2, 1, 0, 0, 0, 0, 1, -2, 1, 0, 0, 0,
        0, 1, -2, 1, 0, 0, 0, 0, 1, -2, 1, 0, 0, 0, 0, 1, -2, 1, 0, 0, 0, 0, 1,
        -2;
    Acts.bottomRightCorner(n / 2, n / 2).setZero();
    Bcts.setZero();
    Bcts(6, 0) = 1;
    Bcts(7, 0) = -1;
    Bcts(8, 1) = 1;
    Bcts(9, 2) = 1;
    Bcts(10, 1) = -1;
    Bcts(11, 2) = -1;
    Omega_bar.setZero();
  }
  void initilizeMPCparameters() {
    // objective matrix
    Q.setIdentity();
    Q *= 10;
    Qf.setIdentity();
    R.setIdentity();
    // S.setRandom();
    S << Eigen::MatrixXd::Identity(m, m), Eigen::MatrixXd::Identity(m, m),
        Eigen::MatrixXd::Identity(m, m), Eigen::MatrixXd::Identity(m, m);
    q.setOnes();
    qf.setZero();
    r.setZero();

    // constraints
    Fx.topLeftCorner(n / 2, n / 2).setIdentity();
    Fx.bottomLeftCorner(n / 2, n / 2).setIdentity();
    Fx.bottomLeftCorner(n / 2, n / 2) = -Fx.bottomLeftCorner(n / 2, n / 2);
    Fx.topRightCorner(n / 2, n / 2).setZero();
    Fx.bottomRightCorner(n / 2, n / 2).setZero();
    fx.setOnes();
    fx *= 4;
    Fu.topRows(m).setIdentity();
    Fu.bottomRows(m).setIdentity();
    Fu.bottomRows(m) = -Fu.bottomRows(m);
    fu.setOnes();
    fu *= 0.5;
    Ff.topRows(n).setIdentity();
    Ff.bottomRows(n).setIdentity();
    Ff.bottomRows(n) = -Ff.bottomRows(n);
    double FinalPositionMax = 2;
    double FinalVelocityMax = 0.5;
    ff << FinalPositionMax * Eigen::Matrix<double, n / 2, 1>::Ones(),
        FinalVelocityMax * Eigen::Matrix<double, n / 2, 1>::Ones(),
        FinalPositionMax * Eigen::Matrix<double, n / 2, 1>::Ones(),
        FinalVelocityMax * Eigen::Matrix<double, n / 2, 1>::Ones();

    // initial condition
    /* we have to ensure the initial x and z satisfy implicit constraints (e.g.
     * Pz<h) */
    x0.setZero();
    z.setZero();

    // Quadratic programming in MPC
    /* H and g */
    H.resize(nz, nz);
    g.resize(nz, 1); // the first row block are not specified
    H.topLeftCorner(m, m) = R;
    H.bottomRightCorner(n, n) = Qf;

    Small_H << Q, S, S.transpose(), R;
    g.topRows(m) = r;
    g.bottomRows(n) = qf;

    for (int i = 0; i != (T - 1); ++i) {
      H.block(m + i * (m + n), m + i * (m + n), m + n, m + n) = Small_H;
      g.segment(m + i * (m + n), m + n) << q, r;
    }

    /* P and h */
    P.resize(T * num_Fu + (T - 1) * num_Fx + num_Ff, nz);
    h.resize(T * num_Fu + (T - 1) * num_Fx + num_Ff, 1);
    P.topLeftCorner(num_Fu, m) = Fu;
    P.bottomRightCorner(num_Ff, n) = Ff;
    h.topRows(num_Fu) = fu;
    h.bottomRows(num_Ff) = ff;
    for (int i = 0; i != (T - 1); ++i) {
      P.block(num_Fu + i * (num_Fu + num_Fx), m + i * (m + n), num_Fx + num_Fu,
              m + n)
          << Fx,
          Eigen::Matrix<double, num_Fx, m>::Zero(),
          Eigen::Matrix<double, num_Fu, n>::Zero(), Fu;
      h.segment(num_Fu + i * (num_Fu + num_Fx), num_Fx + num_Fu) << fx, fu;
    }

    /* C and b */
    C.resize(nmu, nz);
    b.resize(nmu, 1); // the first row block are not specified
    C.topLeftCorner(n, m + n) << -B, Eigen::MatrixXd::Identity(n, n);
    b.topRows(n) = Omega_bar;
    for (int i = 0; i != (T - 1); ++i) {
      C.block(n + i * n, m + i * (m + n), n, 2 * n + m) << -A, -B,
          Eigen::MatrixXd::Identity(n, n);
      b.segment(n + i * n, n) = Omega_bar;
    }
  }

  void generateABdiscrete(int read_index) {
    if (read_index) {
      A = readCSV("Ad.csv", n, n);
      B = readCSV("Bd.csv", n, m);
    } else {
      convertCts2discrete(0.5);
    }
  }
  void convertCts2discrete(double ts) {
    // convert continuous state space model to discrete model using a first
    // order hold model
    auto ActsTs = ts * Acts;
    A = ActsTs.exp();
    B = Acts.fullPivLu().solve(A - Eigen::MatrixXd::Identity(n, n)) * Bcts;
  }
  void convertDense2Sparse() {
    Sparse_H = H.sparseView();
    Sparse_P = P.sparseView();
    Sparse_C = C.sparseView();
  }

  // compute Newton step using schur complement
  void SchurcomputeNewtonStep(const Vectnz &t_z, const Vectnmu &t_mu,
                              const Vectn &t_x0, double t_kappa,
                              Vectnz &t_delta_z, Vectnmu &t_delta_mu) {
    // update g and b
    g.head(m) = r + 2 * S.transpose() * t_x0;
    b.head(n) = A * t_x0 + Omega_bar;
    // compute residuals
    d = h - Sparse_P * t_z;
    d = d.cwiseInverse();
    rd = 2 * Sparse_H * t_z + g + t_kappa * Sparse_P.transpose() * d +
         Sparse_C.transpose() * t_mu;
    rp = Sparse_C * t_z - b;
    // compute big theta
    Eigen::MatrixXd diagd = d.asDiagonal();
    Eigen::SparseMatrix<double> sparse_d = diagd.sparseView();
    Eigen::SparseMatrix<double> diagdP = (sparse_d * Sparse_P).pruned();
    Sparse_Theta =
        2 * Sparse_H + t_kappa * (diagdP.transpose() * diagdP).pruned();
    // compute Newton step using Schur complement
    Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> solver_Theta;
    Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> solver_Y;
    solver_Theta.compute(Sparse_Theta);
    Eigen::SparseMatrix<double, Eigen::ColMajor> Y = solver_Theta.solve(
        Eigen::SparseMatrix<double, Eigen::ColMajor>(Sparse_C.transpose()));
    Y = (Sparse_C * Y).pruned();
    Eigen::VectorXd Beta = solver_Theta.solve(rd);
    Beta = Sparse_C * Beta - rp;
    solver_Y.compute(Y);
    t_delta_mu = solver_Y.solve(-Beta);
    t_delta_z = solver_Theta.solve(-rd - Sparse_C.transpose() * t_delta_mu);
  }
  // compute Newton step using fast MPC algorithm based on sparse pattern
  void fastcomputeNewtonStep(const Vectnz &t_z, const Vectnmu &t_mu,
                             const Vectn &t_x0, double t_Kappa,
                             Vectnz &t_delta_z, Vectnmu &t_delta_mu,
                             double &totalresidualnorm) {
    // the first and last segment of d
    d.head(num_Fu) = (fu - Fu * t_z.head(m)).cwiseInverse();
    d.tail(num_Ff) = (ff - Ff * t_z.tail(n)).cwiseInverse();

    // the first and last segment of rd
    rd.head(m) = 2 * R * t_z.head(m) + r + 2 * S.transpose() * t_x0 +
                 t_Kappa * Fu.transpose() * d.head(num_Fu) -
                 B.transpose() * t_mu.head(n);
    rd.tail(n) = 2 * Qf * t_z.tail(n) + qf +
                 t_Kappa * Ff.transpose() * d.tail(num_Ff) + t_mu.tail(n);

    // the first segment of rp
    rp.head(n) = t_z.segment(m, n) - A * t_x0 - B * t_z.head(m) - Omega_bar;

    // compute both partial norm
    double primal_residual_square = 0;
    double dual_residual_square = 0;
    primal_residual_square += rp.head(n).squaredNorm();
    dual_residual_square += rd.head(m).squaredNorm();
    dual_residual_square += rd.tail(n).squaredNorm();

    // the first block of Theta
    Eigen::MatrixXd diagdFu = d.head(num_Fu).asDiagonal() * Fu;
    Eigen::MatrixXd R_last = 2 * R + t_Kappa * diagdFu.transpose() * diagdFu;
    Eigen::MatrixXd Bar_Q_last = Eigen::MatrixXd::Zero(n, n);
    Eigen::MatrixXd Bar_S_last = Eigen::MatrixXd::Zero(n, m);
    Eigen::MatrixXd L_last = Eigen::MatrixXd::Zero(n, n);
    /* computer the Cholesky factorization of first block in Theta */
    Eigen::MatrixXd Bar_R_zero =
        R_last.llt().solve(Eigen::MatrixXd::Identity(m, m));
    Eigen::MatrixXd Bar_R_last = Bar_R_zero;

    // Eigen::MatrixXd L = lltofR_last.matrixL();
    // the last block of Theta
    Eigen::MatrixXd diagdFf = d.tail(num_Ff).asDiagonal() * Ff;
    Eigen::MatrixXd Q_final = 2 * Qf + t_Kappa * diagdFf.transpose() * diagdFf;
    Eigen::MatrixXd Bar_Q_final =
        Q_final.llt().solve(Eigen::MatrixXd::Identity(n, n));

    // compute Beta
    Eigen::VectorXd Beta(nmu);
    // computer delta_mu
    Eigen::VectorXd Delta_Barmu(nmu);

    // temporary matrix for loop
    Eigen::MatrixXd Theta_temp(mpn, mpn);
    Eigen::MatrixXd Inv_Theta_temp(mpn, mpn);
    Eigen::MatrixXd Bar_Q_ii(n, n);
    Eigen::MatrixXd Bar_S_ii(n, m);
    Eigen::MatrixXd Bar_R_ii(m, m);
    Eigen::MatrixXd Yii(n, n);
    Eigen::MatrixXd Yii_1(n, n);
    Eigen::MatrixXd temp_ASB(n, n);
    Eigen::MatrixXd Lii(n, n);
    Eigen::MatrixXd Li_1i(n, n);
    Eigen::MatrixXd FxTdiagd(n, num_Fx);
    Eigen::MatrixXd FuTdiagd(m, num_Fu);
    Eigen::VectorXd Kappa_rd(mpn);
    Eigen::VectorXd Ctdeltamu(mpn);
    Eigen::VectorXd rd_last1 = Eigen::VectorXd::Zero(n);
    Eigen::VectorXd rd_last2 = Eigen::VectorXd::Zero(m);

    // std::map to store Cholesky factorization results of Theta and Y
    std::map<int, Matmpn, std::less<int>,
             Eigen::aligned_allocator<std::pair<const int, Matmpn>>>
        map4InverseTheta;
    std::map<int, Matnn, std::less<int>,
             Eigen::aligned_allocator<std::pair<const int, Matnn>>>
        map4Lii;
    std::map<int, Matnn, std::less<int>,
             Eigen::aligned_allocator<std::pair<const int, Matnn>>>
        map4Lii_1;

    // start loop

    int index = 0;
    int index_d = 0;
    int index_n = 0;
    for (int i = 0; i != (T - 1); ++i) {
      // update index
      index = i * mpn;
      index_d = i * (num_Fu + num_Fx);
      index_n = i * n;
      // compute d
      d.segment(index_d + num_Fu, num_Fx) =
          (fx - Fx * t_z.segment(m + index, n)).cwiseInverse();
      d.segment(index_d + num_Fu + num_Fx, num_Fu) =
          (fu - Fu * t_z.segment(mpn + index, m)).cwiseInverse();

      // compute rd
      Kappa_rd << t_Kappa * Fx.transpose() *
                          d.segment(index_d + num_Fu, num_Fx) +
                      t_mu.segment(index_n, n) -
                      A.transpose() * t_mu.segment(index_n + n, n),
          t_Kappa * Fu.transpose() *
                  d.segment(index_d + num_Fu + num_Fx, num_Fu) -
              B.transpose() * t_mu.segment(index_n + n, n);
      rd.segment(m + index, mpn) = 2 * Small_H * t_z.segment(m + index, mpn) +
                                   g.segment(m + index, mpn) + Kappa_rd;

      // compute rp
      rp.segment(index_n + n, n) = t_z.segment(index + m + mpn, n) -
                                   A * t_z.segment(index + m, n) -
                                   B * t_z.segment(index + mpn, m) - Omega_bar;

      /* compute residuals */
      primal_residual_square += rp.segment(index_n + n, n).squaredNorm();
      dual_residual_square += rd.segment(m + index, mpn).squaredNorm();

      // compute small theta
      FxTdiagd =
          Fx.transpose() * d.segment(index_d + num_Fu, num_Fx).asDiagonal();
      FuTdiagd = Fu.transpose() *
                 d.segment(index_d + num_Fu + num_Fx, num_Fu).asDiagonal();
      Theta_temp << 2 * Q + t_Kappa * FxTdiagd * FxTdiagd.transpose(), 2 * S,
          2 * S.transpose(), 2 * R + t_Kappa * FuTdiagd * FuTdiagd.transpose();

      /* computer inverse of small theta */
      Inv_Theta_temp =
          Theta_temp.llt().solve(Eigen::MatrixXd::Identity(mpn, mpn));
      /* update map for LLt of small theta */
      map4InverseTheta[i] = Inv_Theta_temp;

      // computer Yii
      temp_ASB = A * Bar_S_last * B.transpose();
      Bar_Q_ii = Inv_Theta_temp.topLeftCorner(n, n);
      Bar_S_ii = Inv_Theta_temp.topRightCorner(n, m);
      Bar_R_ii = Inv_Theta_temp.bottomRightCorner(m, m);
      Yii = A * Bar_Q_last * A.transpose() + temp_ASB + temp_ASB.transpose() +
            B * Bar_R_last * B.transpose() + Bar_Q_ii;
      Yii_1 = -Bar_Q_ii * A.transpose() - Bar_S_ii * B.transpose();

      // compute LLt of Y
      Eigen::LLT<Eigen::MatrixXd> lltofYii(Yii - L_last * L_last.transpose());
      Lii = lltofYii.matrixL();
      Li_1i = Lii.triangularView<Eigen::Lower>().solve(Yii_1);
      Li_1i.transposeInPlace(); // a = a.transpose(); // !!! do NOT do this !!!
      /* update map for LLt of Y */
      map4Lii[i] = Lii;
      map4Lii_1[i] = Li_1i;

      // computer Beta
      Beta.segment(index_n, n) =
          rp.segment(index_n, n) +
          A * (Bar_Q_last * rd_last1 + Bar_S_last * rd_last2) +
          B * (Bar_S_last.transpose() * rd_last1 + Bar_R_last * rd_last2) -
          Bar_Q_ii * rd.segment(m + index, n) -
          Bar_S_ii * rd.segment(m + index + n, m);
      /* update rd_last*/
      rd_last1 = rd.segment(m + index, n);
      rd_last2 = rd.segment(n + m + index, m);

      // update Q,R,S,L
      Bar_Q_last = Bar_Q_ii;
      Bar_S_last = Bar_S_ii;
      Bar_R_last = Bar_R_ii;
      L_last = Li_1i;
    }

    // YTT
    temp_ASB = A * Bar_S_last * B.transpose();
    Yii = A * Bar_Q_last * A.transpose() + temp_ASB + temp_ASB.transpose() +
          B * Bar_R_last * B.transpose() + Bar_Q_final;

    // compute LLt of Y (LTT)
    Eigen::LLT<Eigen::MatrixXd> lltofYii(Yii - L_last * L_last.transpose());
    Lii = lltofYii.matrixL();
    /* update map for LLt of Y */
    map4Lii[T - 1] = Lii;

    // compute final segment of beta
    Beta.tail(n) =
        rp.tail(n) + A * (Bar_Q_last * rd_last1 + Bar_S_last * rd_last2) +
        B * (Bar_S_last.transpose() * rd_last1 + Bar_R_last * rd_last2) -
        Bar_Q_final * rd.tail(n);

    // Forward subsitution to compute delta_mu
    Delta_Barmu.head(n) =
        map4Lii[0].triangularView<Eigen::Lower>().solve(Beta.head(n));
    /* start loop */
    for (int i = 0; i != (T - 1); ++i) {
      index_n = i * n;
      Delta_Barmu.segment(index_n + n, n) =
          map4Lii[i + 1].triangularView<Eigen::Lower>().solve(
              Beta.segment(index_n + n, n) -
              map4Lii_1[i] * Delta_Barmu.segment(index_n, n));
    }

    // backward subsitution to compute delta_mu and delta_z
    /* compute the final segment of delta_mu and delta_z */
    t_delta_mu.tail(n) =
        map4Lii[T - 1].transpose().triangularView<Eigen::Upper>().solve(
            Delta_Barmu.tail(n));
    t_delta_z.tail(n) = -Bar_Q_final * (rd.tail(n) + t_delta_mu.tail(n));

    /* start loop */
    for (int i = 0; i != (T - 1); ++i) {
      int j = T - i - 1;
      index_n = j * n;
      index = j * mpn;

      t_delta_mu.segment(index_n - n, n) =
          map4Lii[j - 1].transpose().triangularView<Eigen::Upper>().solve(
              Delta_Barmu.segment(index_n - n, n) -
              map4Lii_1[j - 1].transpose() * t_delta_mu.segment(index_n, n));
      Ctdeltamu << t_delta_mu.segment(index_n - n, n) -
                       A.transpose() * t_delta_mu.segment(index_n, n),
          -B.transpose() * t_delta_mu.segment(index_n, n);
      t_delta_z.segment(index - n, mpn) =
          -map4InverseTheta[j - 1] * (rd.segment(index - n, mpn) + Ctdeltamu);
    }
    /* compute the first segment of delta_z */
    t_delta_z.head(m) =
        -Bar_R_zero * (rd.head(m) - B.transpose() * t_delta_mu.head(n));

    // compute the l2 norm of primal and dual residuals
    totalresidualnorm =
        sqrt(primal_residual_square + dual_residual_square); // squared l2 norm
    /* check if the implicit constraints are satisfied */
    if ((d.array() < residuals_tolerance).any()) {
      totalresidualnorm *= 1000;
    }
  }
  // conmpute residuals
  double computeResidualsAndNorm(const Vectnz &t_z, const Vectnmu &t_mu,
                                 const Vectn &t_x0, double t_Kappa) {
    double primal_residual_square = 0;
    double dual_residual_square = 0;
    // the first and last segment of d
    d.head(num_Fu) = (fu - Fu * t_z.head(m)).cwiseInverse();
    d.tail(num_Ff) = (ff - Ff * t_z.tail(n)).cwiseInverse();

    // the first and last segment of rd
    rd.head(m) = 2 * R * t_z.head(m) + r + 2 * S.transpose() * t_x0 +
                 t_Kappa * Fu.transpose() * d.head(num_Fu) -
                 B.transpose() * t_mu.head(n);
    rd.tail(n) = 2 * Qf * t_z.tail(n) + qf +
                 t_Kappa * Ff.transpose() * d.tail(num_Ff) + t_mu.tail(n);

    // the first segment of rp
    rp.head(n) = t_z.segment(m, n) - A * t_x0 - B * t_z.head(m) - Omega_bar;

    // compute both partial norm
    primal_residual_square += rp.head(n).squaredNorm();
    dual_residual_square += rd.head(m).squaredNorm();
    dual_residual_square += rd.tail(n).squaredNorm();
    // temporary matrix for loop
    Eigen::VectorXd Kappa_rd(mpn);

    int index = 0;
    int index_d = 0;
    int index_n = 0;
    for (int i = 0; i != (T - 1); ++i) {
      // update index
      index = i * mpn;
      index_d = i * (num_Fu + num_Fx);
      index_n = i * n;
      // compute d
      d.segment(index_d + num_Fu, num_Fx) =
          (fx - Fx * t_z.segment(m + index, n)).cwiseInverse();
      d.segment(index_d + num_Fu + num_Fx, num_Fu) =
          (fu - Fu * t_z.segment(mpn + index, m)).cwiseInverse();

      // compute rd
      Kappa_rd << t_Kappa * Fx.transpose() *
                          d.segment(index_d + num_Fu, num_Fx) +
                      t_mu.segment(index_n, n) -
                      A.transpose() * t_mu.segment(index_n + n, n),
          t_Kappa * Fu.transpose() *
                  d.segment(index_d + num_Fu + num_Fx, num_Fu) -
              B.transpose() * t_mu.segment(index_n + n, n);
      rd.segment(m + index, mpn) = 2 * Small_H * t_z.segment(m + index, mpn) +
                                   g.segment(m + index, mpn) + Kappa_rd;

      // compute rp
      rp.segment(index_n + n, n) = t_z.segment(index + m + mpn, n) -
                                   A * t_z.segment(index + m, n) -
                                   B * t_z.segment(index + mpn, m) - Omega_bar;

      // compute residuals
      primal_residual_square += rp.segment(index_n + n, n).squaredNorm();
      dual_residual_square += rd.segment(m + index, mpn).squaredNorm();
    }
    // compute the l2 norm of primal and dual residuals
    double totalresidualnorm =
        sqrt(primal_residual_square + dual_residual_square); // squared l2 norm
    /* check if the implicit constraints are satisfied */
    if ((d.array() < residuals_tolerance).any()) {
      totalresidualnorm *= 1000;
    }
    return totalresidualnorm;
  }
  // compute step size using backtracking line search
  void computeStepsizeAndNextState(Vectnz &t_z, Vectnmu &t_mu,
                                   const Vectnz &t_delta_z,
                                   const Vectnmu &t_delta_mu, const Vectn &t_x0,
                                   double t_Kappa, double last_residual) {
    double step_size = 1;
    Vectnz z_temp = t_z + step_size * t_delta_z;
    Vectnmu mu_temp = t_mu + step_size * t_delta_mu;
    while (computeResidualsAndNorm(z_temp, mu_temp, t_x0, t_Kappa) >
           (1 - alpha * step_size) * last_residual) {
      step_size *= beta;
      z_temp = t_z + step_size * t_delta_z;
      mu_temp = t_mu + step_size * t_delta_mu;
    }
    t_z = z_temp;
    t_mu = mu_temp;
  }
  // warm start for MPC
  void warmstart(const Vectm &u_star, const Vectn &x_star, Vectnz &init_z) {
    init_z << init_z.tail(nz - mpn), u_star, x_star;
  }
  void warmstart(Vectnz &init_z) { init_z.setZero(); }
  // basic infeasible start primal barrier method to solve QP in MPC
  void basicQP4MPC(const Vectn &t_x0, Vectm &t_u, double t_kappa = 1) {
    double duality_gap = 0;
    while (true) {
      // centering step
      double totalresidualnorm = 0;
      while (true) {
        // compute the primal and dual Newton steps
        fastcomputeNewtonStep(z, mu, t_x0, t_kappa, delta_z, delta_mu,
                              totalresidualnorm);
        fastcount++;
        if (totalresidualnorm <= tolerance)
          break;
        else
          // backtracking line search for step length
          computeStepsizeAndNextState(z, mu, delta_z, delta_mu, t_x0, t_kappa,
                                      totalresidualnorm);
      }
      // update the outer loop
      duality_gap = t_kappa * (T * num_Fu + (T - 1) * num_Fx + num_Ff);
      if (duality_gap < required_duality_gap) {
        t_u = z.head(m);
        // warmstart(t_u, z.segment(m, n), z);
        warmstart(z);
        break;
      } else
        t_kappa *= kappa_rate;
    }
  }
  // approximate primal barrier method to solve MPC
  void approximateQP4MPC(const Vectn &t_x0, Vectm &t_u, double t_kappa = 0.1) {
    int Kmax = 5;
    for (int i = 0; i != Kmax; ++i) {
      // centering step
      double totalresidualnorm = 0;
      while (true) {
        // compute the primal and dual Newton steps
        fastcomputeNewtonStep(z, mu, t_x0, t_kappa, delta_z, delta_mu,
                              totalresidualnorm);
        fastcount++;
        if (totalresidualnorm <= tolerance)
          break;
        else
          // backtracking line search for step length
          computeStepsizeAndNextState(z, mu, delta_z, delta_mu, t_x0, t_kappa,
                                      totalresidualnorm);
      }
    }
    t_u = z.head(m);
    // warmstart(t_u, z.segment(m, n), z);
    warmstart(z);
  }
  // update or insert the sparse matrix
  void insertSparseMatrix(Eigen::SparseMatrix<double, Eigen::ColMajor> &smat,
                          int start_row, int start_col,
                          const Eigen::MatrixXd &dmat) {
    // insert() assumes that the element does not already exist, but this
    // operation is too slow!!
    int rows = dmat.rows();
    int cols = dmat.cols();
    for (int i = 0; i != rows; ++i)
      for (int j = 0; j != cols; ++j) {
        smat.insert(start_row + i, start_col + j) = dmat(i, j);
      }
  }

  // PID controller
  void pidcontroller() {
    // to do
  }

  void kalmanfilter() {
    // to do
  }
  // control allocation with singularity avoidance
  void thrustallocation() {}
  // read data from csv file and pass to Eigen matrix
  Eigen::MatrixXd readCSV(const std::string &file, int rows, int cols) {

    std::ifstream in(file);
    std::string line;

    int row = 0;
    int col = 0;

    Eigen::MatrixXd res = Eigen::MatrixXd(rows, cols);

    if (in.is_open()) {

      while (std::getline(in, line)) {

        char *ptr = (char *)line.c_str();
        int len = line.length();

        col = 0;

        char *start = ptr;
        for (int i = 0; i < len; i++) {

          if (ptr[i] == ',') {
            res(row, col++) = atof(start);
            start = ptr + i + 1;
          }
        }
        res(row, col) = atof(start);
        row++;
      }

      in.close();
    }
    return res;
  }
  // write Eigen matrix into csv file
  void write2csvfile(const std::string &name, const Eigen::MatrixXd &matrix) {
    const static Eigen::IOFormat CSVFormat(Eigen::StreamPrecision,
                                           Eigen::DontAlignCols, ", ", "\n");
    std::ofstream file(name.c_str());
    if (file.is_open()) {
      file << matrix.format(CSVFormat);
    }
  }
};

#endif /* _CONTROLLER_H_ */
