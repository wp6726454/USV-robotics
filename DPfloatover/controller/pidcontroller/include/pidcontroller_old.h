/*
***********************************************************************
* pidcontrollerold.hpp:
* function for observer, controller and optimization (e.g. Kalman filter,
* PID controller, and thruster allocation)
* This header file can be read by C++ compilers
*
* by Hu.ZH(Mr.SJTU)
***********************************************************************
*/

#ifndef _PIDCONTROLLEROLD_H_
#define _PIDCONTROLLEROLD_H_

#include <math.h>
#include <stdio.h>
#include <Eigen/Core>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <unsupported/Eigen/MatrixFunctions>
#include "../../../include/constants.h"
<<<<<<< HEAD
#include "/home/scar1et/Coding/CPP1X/USV/timer/timecounter.hpp"
=======
#include "timecounter.hpp"
>>>>>>> real time control
#include "vesselparameter.h"

void write2csvfile(const std::string &, const Eigen::MatrixXd &);

class pidcontrollerold {
  friend void output2csv(const pidcontrollerold &, const std::string &);
  friend void outputmatrices(const pidcontrollerold &);

 public:
  explicit pidcontrollerold(const vessel &_vessel)
      : m(_vessel.m),
        n(_vessel.n),
        nz(_vessel.nz),
        num_constraints(_vessel.num_constraints),
        nmu(_vessel.nmu),
        ny(_vessel.ny),
        max_positive_forcex(_vessel.max_positive_forcex),
        max_negative_forcex(_vessel.max_negative_forcex),
        max_positive_forcey(_vessel.max_positive_forcey),
        max_negative_forcey(_vessel.max_negative_forcey),
        max_positive_momentz(_vessel.max_positive_momentz),
        max_negative_momentz(_vessel.max_negative_momentz) {
    initilizeTAparameters();
    initializeQPparameters();
    initializeSolverparameters();
  }
  ~pidcontrollerold() {}

  void basicQP_onestep(int index = 1) {
    timecounter t1("one step");
    // u.setRandom();
    // u = u.cwiseAbs();
    // alpha.setRandom();
    // alpha *= 10;
    u.setConstant(0.021);
    alpha.setConstant(0.01);
    tau << 2.7, 0, 0;
    // update parameters for each-step thruster allocation
    updateTAparameters(alpha, u);
    // update tau
    computeGeneralizedForce_PID();
    // update parameters for each-step QP
    updateQPparameters(tau);
    // QP solver
    if (index)
      basicQPsolver_interior();
    else
      basicQPsolver_barrier();
    // update alpha and u
    updateAlphaandU();
  }

  void basicQP_multistep(int index = 1) {
    // desired forces
    Eigen::MatrixXd total_tau = Eigen::MatrixXd::Zero(n, totalstep);
    double angle = 0;
    for (int i = 0; i != 30; ++i) {
      angle = (i + 1) * M_PI / 15;
      total_tau(2, i + 70) = 2.3 * sin(angle) + 0.1 * std::rand() / RAND_MAX;
    }
    total_tau.block(0, 0, 1, 100) = Eigen::MatrixXd::Constant(1, 100, 3) +
                                    0.2 * Eigen::MatrixXd::Random(1, 100);
    total_tau.block(0, 100, 1, 100) = Eigen::MatrixXd::Constant(1, 100, 3) +
                                      0.2 * Eigen::MatrixXd::Random(1, 100);
    total_tau.row(1) = 0.2 * Eigen::MatrixXd::Random(1, totalstep);
    write2csvfile("total.csv", total_tau);
    initializethruster();
    timecounter t1("multi step");

    if (index) {
      for (int i = 0; i != totalstep; ++i) {
        // save alpha and u
        save_u.col(i) = u;
        save_alpha.col(i) = alpha;
        save_Balphau.col(i) = B_alpha * u;
        // update parameters for each-step thruster allocation
        updateTAparameters(alpha, u);
        // update tau
        // computeGeneralizedForce_PID();
        // update parameters for each-step QP
        updateQPparameters(total_tau.col(i));
        // QP solver
        basicQPsolver_interior();
        // update alpha and u
        updateAlphaandU();
      }

    } else {
      for (int i = 0; i != totalstep; ++i) {
        std::cout << i << std::endl;
        // save alpha and u
        save_u.col(i) = u;
        save_alpha.col(i) = alpha;
        save_Balphau.col(i) = B_alpha * u;
        // update parameters for each-step thruster allocation
        updateTAparameters(alpha, u);
        // update tau
        // computeGeneralizedForce_PID();
        // update parameters for each-step QP
        updateQPparameters(total_tau.col(i));
        // QP solver
        basicQPsolver_barrier();
        // update alpha and u
        updateAlphaandU();
      }
    }
  }
  void ApproQP_multistep(int index = 1) {
    // desired forces
    Eigen::MatrixXd total_tau = Eigen::MatrixXd::Zero(n, totalstep);
    double angle = 0;
    for (int i = 0; i != 30; ++i) {
      angle = (i + 1) * M_PI / 15;
      total_tau(2, i + 20) = 2.3 * sin(angle) + 0.1 * std::rand() / RAND_MAX;
    }
    total_tau.block(0, 0, 1, 100) = Eigen::MatrixXd::Constant(1, 100, 3) +
                                    0.2 * Eigen::MatrixXd::Random(1, 100);
    total_tau.block(0, 100, 1, 100) = Eigen::MatrixXd::Constant(1, 100, 3) +
                                      0.2 * Eigen::MatrixXd::Random(1, 100);
    total_tau.row(1) = 0.2 * Eigen::MatrixXd::Random(1, totalstep);
    write2csvfile("total.csv", total_tau);
    initializethruster();
    timecounter t1("multi step");

    if (index) {
      for (int i = 0; i != totalstep; ++i) {
        // save alpha and u
        save_u.col(i) = u;
        save_alpha.col(i) = alpha;
        save_Balphau.col(i) = B_alpha * u;
        // update parameters for each-step thruster allocation
        updateTAparameters(alpha, u);
        // update tau
        // computeGeneralizedForce_PID();
        // update parameters for each-step QP
        updateQPparameters(total_tau.col(i));
        // QP solver
        basicQPsolver_interior();
        // update alpha and u
        updateAlphaandU();
      }

    } else {
      for (int i = 0; i != totalstep; ++i) {
        // save alpha and u
        save_u.col(i) = u;
        save_alpha.col(i) = alpha;
        save_Balphau.col(i) = B_alpha * u;
        // update parameters for each-step thruster allocation
        updateTAparameters(alpha, u);
        // update tau
        // computeGeneralizedForce_PID();
        // update parameters for each-step QP
        updateQPparameters(total_tau.col(i));
        // QP solver
        approximateQPsolver_barrier();
        // update alpha and u
        updateAlphaandU();
      }
    }
  }
  // full manual control mode
  void fullymanualcontrol(int xforce, int yforce, int zmoment) {
    // static int count = 0;
    // save alpha and u
    // save_u.col(count) = u;
    // save_alpha.col(count) = alpha;
    // save_Balphau.col(count) = B_alpha * u;
    // update parameters for each-step thruster allocation
    updateTAparameters(alpha, u);
    // update tau
    setGeneralizeForce(xforce, yforce, zmoment);
    // save tau
    // save_tau.col(count) = tau;
    // update parameters for each-step QP
    updateQPparameters(tau);
    // QP solver
    basicQPsolver_interior();
    // update alpha and u
    updateAlphaandU();

    // ++count;
  }

  // initialize thruster states
  void initializethruster() {
    u.setConstant(0.001);
    alpha.setZero();
  }

 private:
  int m;
  int n;
  int nz;
  int num_constraints;  // the number of constraints
  int nmu;
  int ny;
  // parameters in full PID controller

  // parameters in full manual controller, which gives absolute value of max
  // generalized force in the each direction
  double max_positive_forcex;
  double max_negative_forcex;
  double max_positive_forcey;
  double max_negative_forcey;
  double max_positive_momentz;
  double max_negative_momentz;
  // parameters in Thruster allcation
  Eigen::MatrixXd Q;
  double Q_K = 10;
  Eigen::MatrixXd Omega;
  double Omega_K = 1;
  Eigen::VectorXd lx;
  Eigen::VectorXd ly;
  Eigen::MatrixXd B_alpha;
  Eigen::VectorXd alpha;        // rad
  Eigen::VectorXd delta_alpha;  // rad
  Eigen::VectorXd u;
  Eigen::VectorXd delta_u;
  Eigen::VectorXd d_rho;       // derivative of rho term
  Eigen::MatrixXd d_Balpha_u;  // Jocobian matrix of Balpha times u
  Eigen::VectorXd g_deltau;
  Eigen::MatrixXd Q_deltau;
  Eigen::VectorXd tau;
  Eigen::VectorXd Umax;
  Eigen::VectorXd Alphamax;  // rad
  Eigen::VectorXd Umin;
  Eigen::VectorXd Alphamin;  // rad
  Eigen::VectorXd delta_Umax;
  Eigen::VectorXd delta_Alphamax;  // rad
  Eigen::VectorXd delta_Umin;
  Eigen::VectorXd delta_Alphamin;  // rad

  // parameters in QP based on above parameters
  Eigen::MatrixXd H;
  Eigen::VectorXd g;
  Eigen::MatrixXd C;
  Eigen::VectorXd b;
  Eigen::MatrixXd P;
  Eigen::VectorXd h;

  // real-time data for each QP using primal dual barrier method
  Eigen::VectorXd z;
  Eigen::VectorXd mu;
  Eigen::VectorXd lambda;
  Eigen::VectorXd delta_z;
  Eigen::VectorXd delta_mu;
  Eigen::VectorXd delta_lambda;
  Eigen::VectorXd delta_y;
  Eigen::VectorXd rd;
  Eigen::VectorXd rc;
  Eigen::VectorXd rp;
  Eigen::VectorXd total_residual;
  Eigen::VectorXd d;
  Eigen::MatrixXd Theta;
  Eigen::MatrixXd Bigtheta;

  // iteration constant in QP
  double alpha_backtrack = 0.01;
  double beta_backtrack = 0.5;
  double tolerance = 1e-4;
  double residuals_tolerance = 1e-5;
  double required_duality_gap = 1e-3;
  double kappa_rate = 0.1;

  // linearized parameters
  double derivative_dx = 1e-5;  // step size of the derivative

  // data saved for validation and viewer
  const int totalstep = 200;
  Eigen::MatrixXd save_u = Eigen::MatrixXd::Zero(m, totalstep);
  Eigen::MatrixXd save_alpha = Eigen::MatrixXd::Zero(m, totalstep);
  Eigen::MatrixXd save_Balphau = Eigen::MatrixXd::Zero(n, totalstep);
  Eigen::MatrixXd save_tau = Eigen::MatrixXd::Zero(n, totalstep);
  // initialize parameters for thruster allocation
  void initilizeTAparameters() {
    Q.setIdentity(n, n);
    Q *= Q_K;
    Omega.setIdentity(m, m);
    Omega *= Omega_K;
    lx.setZero(m);
    ly.setZero(m);
    lx << 20, 20, -20, -20;
    ly << 5, -5, 5, -5;
    B_alpha.setZero(n, m);
    alpha.setZero(m);
    delta_alpha.setZero(m);
    u.setZero(m);
    delta_u.setZero(m);
    d_rho.setZero(m);
    d_Balpha_u.setZero(n, m);
    g_deltau.setZero(m);
    Q_deltau.setZero(m, m);
    tau.setZero(n);
    double max_u = 1;
    double min_u = 0;
    double maxdelta_u = 0.02;
    double mindelta_u = -0.02;
    Umax.setZero(m);
    Umin.setZero(m);
    delta_Umax.setZero(m);
    delta_Umin.setZero(m);
    Umax << max_u, max_u, max_u, max_u;
    Umin << min_u, min_u, min_u, min_u;
    delta_Umax << maxdelta_u, maxdelta_u, maxdelta_u, maxdelta_u;
    delta_Umin << mindelta_u, mindelta_u, mindelta_u, mindelta_u;

    double maxalpha = 25 * M_PI / 180;
    double maxdelta_alpha = 1 * M_PI / 180;
    double minalpha = -25 * M_PI / 180;
    double mindelta_alpha = -1 * M_PI / 180;
    Alphamax.setZero(m);
    Alphamin.setZero(m);
    delta_Alphamax.setZero(m);
    delta_Alphamin.setZero(m);
    Alphamax << maxalpha, maxalpha, maxalpha, maxalpha;
    Alphamin << minalpha, minalpha, minalpha, minalpha;
    delta_Alphamax << maxdelta_alpha, maxdelta_alpha, maxdelta_alpha,
        maxdelta_alpha;
    delta_Alphamin << mindelta_alpha, mindelta_alpha, mindelta_alpha,
        mindelta_alpha;
  }
  // initialize parameters for Quadratic programming
  void initializeQPparameters() {
    // H
    H.setZero(nz, nz);
    H.block(m, m, m, m) = Omega;
    H.block(2 * m, 2 * m, n, n) = Q;
    g.setZero(nz);
    C.setZero(n, nz);
    C.block(0, 2 * m, n, n).setIdentity();
    b.setZero(n);
    P.setZero(num_constraints, nz);
    P.block(0, 0, m, m).setIdentity();
    P.block(m, 0, m, m).setIdentity();
    P.block(m, 0, m, m) *= -1;
    P.block(2 * m, m, m, m).setIdentity();
    P.block(3 * m, m, m, m).setIdentity();
    P.block(3 * m, m, m, m) *= -1;
    h.setZero(num_constraints);
  }
  // initilize parameter for QP solver
  void initializeSolverparameters() {
    z.setZero(nz);
    mu.setZero(nmu);
    lambda.setZero(num_constraints);
    delta_z.setZero(nz);
    delta_mu.setZero(nmu);
    delta_lambda.setZero(num_constraints);
    delta_y.setZero(ny);
    rd.setZero(nz);
    rc.setZero(num_constraints);
    rp.setZero(n);
    total_residual.setZero(ny);
    d.setZero(num_constraints);
    Theta.setZero(nz, nz);
    Bigtheta.setZero(ny, ny);
  }

  // specify all the generalize force using gamepad
  void setGeneralizeForce(int xforce, int yforce, int zmoment) {
    // specify force in x direction
    if (xforce > 0)
      tau(0) = xforce * max_positive_forcex;
    else
      tau(0) = xforce * max_negative_forcex;

    // specify force in y direction
    if (yforce > 0)
      tau(1) = yforce * max_positive_forcey;
    else
      tau(1) = yforce * max_negative_forcey;

    // specify moment in z direction
    if (zmoment > 0)
      tau(2) = zmoment * max_positive_momentz;
    else
      tau(2) = zmoment * max_negative_momentz;
  }

  // PID controller
  void computeGeneralizedForce_PID() { tau.setRandom(); }
  // calculate Balpha as function of alpha
  Eigen::MatrixXd calculateBalpha(const Eigen::VectorXd &t_alpha) {
    Eigen::MatrixXd _B_alpha = Eigen::MatrixXd::Zero(n, m);
    double angle = 0;
    double t_cos = 0;
    double t_sin = 0;
    for (int i = 0; i != m; ++i) {
      angle = t_alpha(i);
      t_cos = cos(angle);
      t_sin = sin(angle);
      _B_alpha(0, i) = t_cos;
      _B_alpha(1, i) = t_sin;
      _B_alpha(2, i) = -ly(i) * t_cos + lx(i) * t_sin;
    }
    return _B_alpha;
  }
  // calculate the rho term in thruster allocation
  double calculateRhoTerm(const Eigen::VectorXd &t_alpha, double epsilon = 0.01,
                          double rho = 10) {
    // timecounter t1("deter");
    auto _B_alpha = calculateBalpha(t_alpha);
    Eigen::MatrixXd BBT(n, n);
    BBT = _B_alpha * _B_alpha.transpose();
    return rho / (epsilon + BBT.determinant());
  }
  // calculate Jacobian using central difference
  void calculateJocobianRhoTerm(const Eigen::VectorXd &t_alpha) {
    for (int i = 0; i != m; ++i) {
      auto alpha_plus = t_alpha;
      auto alpha_minus = t_alpha;
      alpha_plus(i) += derivative_dx;
      alpha_minus(i) -= derivative_dx;
      d_rho(i) =
          centraldifference(calculateRhoTerm(alpha_plus),
                            calculateRhoTerm(alpha_minus), derivative_dx);
    }
  }
  // calculate the Balpha u term
  Eigen::VectorXd calculateBalphau(const Eigen::VectorXd &t_alpha,
                                   const Eigen::VectorXd &t_u) {
    return calculateBalpha(t_alpha) * t_u;
  }
  // calculate derivative of Balpha times u
  void calculateJocobianBalphaU(const Eigen::VectorXd &t_alpha,
                                const Eigen::VectorXd &t_u) {
    for (int i = 0; i != m; ++i) {
      auto alpha_plus = t_alpha;
      auto alpha_minus = t_alpha;
      alpha_plus(i) += derivative_dx;
      alpha_minus(i) -= derivative_dx;
      d_Balpha_u.col(i) = (calculateBalphau(alpha_plus, t_u) -
                           calculateBalphau(alpha_minus, t_u)) /
                          (2 * derivative_dx);
    }
  }
  // calculate g_deltau and Q_deltau
  void calculateDeltauQ(const Eigen::VectorXd &t_u) {
    Eigen::VectorXd d_utemp(m);
    d_utemp = t_u.cwiseSqrt();
    g_deltau = 1.5 * d_utemp;
    Eigen::VectorXd Q_temp = Eigen::VectorXd::Zero(m);
    Q_temp = 0.75 * d_utemp.cwiseInverse();
    Q_deltau = Q_temp.asDiagonal();
  }
  // update parameters in thruster allocation for each time step
  void updateTAparameters(const Eigen::VectorXd &t_alpha,
                          const Eigen::VectorXd &t_u) {
    B_alpha = calculateBalpha(t_alpha);
    calculateJocobianRhoTerm(t_alpha);
    calculateJocobianBalphaU(t_alpha, t_u);
    calculateDeltauQ(t_u);
  }
  // update parameters in QP for each time step
  void updateQPparameters(const Eigen::VectorXd &t_tau) {
    // update H
    H.block(0, 0, m, m) = Q_deltau;
    // update g
    g.segment(0, m) = g_deltau;
    g.segment(m, m) = d_rho;
    // update C
    C.block(0, 0, n, m) = B_alpha;
    C.block(0, m, n, m) = d_Balpha_u;
    // update b
    b = t_tau - B_alpha * u;
    // update h
    h.segment(0, m) = findmin(delta_Umax, Umax - u);
    h.segment(m, m) = -findmax(delta_Umin, Umin - u);
    h.segment(2 * m, m) = findmin(delta_Alphamax, Alphamax - alpha);
    h.segment(3 * m, m) = -findmax(delta_Alphamin, Alphamin - alpha);
  }
  // update alpha and u using computed delta_alpha and delta_u
  void updateAlphaandU() {
    u += delta_u;
    alpha += delta_alpha;
  }
  // compute residuals in QP for primal-dual barrier method
  double computeResidualsAndNorm(const Eigen::VectorXd &t_z,
                                 const Eigen::VectorXd &t_mu, double t_Kappa) {
    auto d4negative = h - P * t_z;
    // compute d
    d = d4negative;
    d = d.cwiseInverse();
    // compute rd and rp
    rd = 2 * H * t_z + g + t_Kappa * P.transpose() * d + C.transpose() * t_mu;
    rp = C * t_z - b;
    // compute L2 norm of primal and dual residuals
    double totalresidual =
        sqrt(rp.squaredNorm() + rd.squaredNorm());  // squared l2 norm
    /* check if the implicit constraints are satisfied */
    if ((d4negative.array() < -residuals_tolerance).any()) {
      // std::cout << "negative" << std::endl;
      totalresidual *= 1000;
    }
    return totalresidual;
  }
  // compute residuals in QP for primal-dual interior point method
  double computeResidualsAndNorm(const Eigen::VectorXd &t_z,
                                 const Eigen::VectorXd &t_mu,
                                 const Eigen::VectorXd &t_lambda,
                                 double t_Kappa,
                                 double &max_dualprimalresidual) {
    // compute vector to check implicit contraints
    Eigen::VectorXd d4negative = Eigen::VectorXd::Zero(2 * num_constraints);
    d4negative << h - P * t_z, t_lambda;
    // compute rd and rp
    rd = 2 * H * t_z + g + P.transpose() * t_lambda + C.transpose() * t_mu;
    rc = t_lambda.asDiagonal() * (h - P * t_z) -
         Eigen::VectorXd::Constant(num_constraints, t_Kappa);
    rp = C * t_z - b;
    // compute total residuals
    total_residual << rd, rc, rp;
    // compute L2 norm of primal and dual residuals
    double primalresidualnorm_square = rp.squaredNorm();
    double dualresidualnorm_square = rd.squaredNorm();
    max_dualprimalresidual =
        sqrt(std::max(primalresidualnorm_square, dualresidualnorm_square));
    double totalresidualnorm = sqrt(primalresidualnorm_square +
                                    dualresidualnorm_square + rc.squaredNorm());
    /* check if the implicit constraints are satisfied */
    if ((d4negative.array() < -residuals_tolerance).any()) {
      totalresidualnorm *= 1000;
    }
    return totalresidualnorm;
  }
  // compute Newton step using schur complement in primal-dual barrier method
  void SchurcomputeNewtonStep(const Eigen::VectorXd &t_z,
                              const Eigen::VectorXd &t_mu, double t_kappa,
                              Eigen::VectorXd &t_delta_z,
                              Eigen::VectorXd &t_delta_mu,
                              double &totalresidualnorm) {
    // compute residuals
    totalresidualnorm = computeResidualsAndNorm(t_z, t_mu, t_kappa);
    // compute theta
    Eigen::MatrixXd diagd(num_constraints, num_constraints);
    diagd = d.asDiagonal();
    Eigen::MatrixXd diagdP(num_constraints, nz);
    diagdP = diagd * P;
    Theta = 2 * H + t_kappa * diagdP.transpose() * diagdP;
    // compute Newton step using schur complement
    /* compute inverse of theta */
    auto Inv_Theta = Theta.llt().solve(Eigen::MatrixXd::Identity(nz, nz));
    /* compute Y */
    Eigen::MatrixXd Y(n, n);
    Y = C * Inv_Theta * C.transpose();
    Eigen::VectorXd Beta(n);
    Beta = rp - C * Inv_Theta * rd;
    /* compute delta_mu */
    t_delta_mu = Y.llt().solve(Beta);
    /* compute delta_z */
    t_delta_z = -Inv_Theta * (rd + C.transpose() * t_delta_mu);
  }
  // compute Newton step in primal-dual interior point method
  void computeNewtonStep(const Eigen::VectorXd &t_z,
                         const Eigen::VectorXd &t_mu,
                         const Eigen::VectorXd &t_lambda, double t_kappa,
                         Eigen::VectorXd &t_delta_z,
                         Eigen::VectorXd &t_delta_mu,
                         Eigen::VectorXd &t_delta_lamdba,
                         double &t_totalresidualnorm,
                         double &t_maxdualprimalresidual) {
    // compute residuals
    t_totalresidualnorm = computeResidualsAndNorm(t_z, t_mu, t_lambda, t_kappa,
                                                  t_maxdualprimalresidual);
    // compute Big theta
    Bigtheta.topLeftCorner(nz, nz) = 2 * H;
    Bigtheta.block(0, nz, nz, num_constraints) = P.transpose();
    Bigtheta.topRightCorner(nz, n) = C.transpose();
    Bigtheta.block(nz, 0, num_constraints, nz) = t_lambda.asDiagonal() * P;
    Bigtheta.block(nz, 0, num_constraints, nz) *= -1;
    Bigtheta.block(nz, nz, num_constraints, num_constraints) =
        (h - P * t_z).asDiagonal();
    Bigtheta.bottomLeftCorner(n, nz) = C;

    // compute Newton step
    Eigen::VectorXd _delta_y = Eigen::VectorXd::Zero(ny);
    _delta_y = -Bigtheta.partialPivLu().solve(total_residual);
    t_delta_z = _delta_y.head(nz);
    t_delta_lamdba = _delta_y.segment(nz, num_constraints);
    t_delta_mu = _delta_y.tail(nmu);
  }
  // compute step size using backtracking line search in barrier method
  void computeStepsizeAndNextState(Eigen::VectorXd &t_z, Eigen::VectorXd &t_mu,
                                   const Eigen::VectorXd &t_delta_z,
                                   const Eigen::VectorXd &t_delta_mu,
                                   double t_Kappa, double last_residual) {
    double step_size = 1;
    Eigen::VectorXd z_temp = Eigen::VectorXd::Zero(nz);
    Eigen::VectorXd mu_temp = Eigen::VectorXd::Zero(nmu);
    z_temp = t_z + step_size * t_delta_z;
    mu_temp = t_mu + step_size * t_delta_mu;
    while (computeResidualsAndNorm(z_temp, mu_temp, t_Kappa) >
           (1 - alpha_backtrack * step_size) * last_residual) {
      step_size *= beta_backtrack;
      z_temp = t_z + step_size * t_delta_z;
      mu_temp = t_mu + step_size * t_delta_mu;
    }
    t_z = z_temp;
    t_mu = mu_temp;
  }
  // compute step size using backtracking line search in interior point method
  void computeStepsizeAndNextState(Eigen::VectorXd &t_z, Eigen::VectorXd &t_mu,
                                   Eigen::VectorXd &t_lambda,
                                   const Eigen::VectorXd &t_delta_z,
                                   const Eigen::VectorXd &t_delta_mu,
                                   const Eigen::VectorXd &t_delta_lambda,
                                   double t_Kappa, double last_residual) {
    double step_size = 1;
    double max_dualprimalresidual = 0;
    Eigen::VectorXd z_temp = Eigen::VectorXd::Zero(nz);
    Eigen::VectorXd mu_temp = Eigen::VectorXd::Zero(nmu);
    Eigen::VectorXd lambda_temp = Eigen::VectorXd::Zero(ny);
    z_temp = t_z + step_size * t_delta_z;
    mu_temp = t_mu + step_size * t_delta_mu;
    lambda_temp = t_lambda + step_size * t_delta_lambda;
    while (computeResidualsAndNorm(z_temp, mu_temp, lambda_temp, t_Kappa,
                                   max_dualprimalresidual) >
           (1 - alpha_backtrack * step_size) * last_residual) {
      step_size *= beta_backtrack;
      z_temp = t_z + step_size * t_delta_z;
      mu_temp = t_mu + step_size * t_delta_mu;
      lambda_temp = t_lambda + step_size * t_delta_lambda;
    }
    t_z = z_temp;
    t_mu = mu_temp;
    t_lambda = lambda_temp;
  }
  // warm start for each step in thruster allocation for barrier method
  void warmstart(Eigen::VectorXd &init_z, Eigen::VectorXd &init_mu) {
    init_z.setZero();
    init_mu.setZero();
  }
  // warm start for each step in thruster allocation for interior point method
  void warmstart(Eigen::VectorXd &init_z, Eigen::VectorXd &init_mu,
                 Eigen::VectorXd &init_lambda) {
    init_z.setZero();
    init_mu.setZero();
    init_lambda.setConstant(0.01);
  }
  // basic infeasible start primal barrier method to solve QP
  void basicQPsolver_barrier(double t_kappa = 1) {
    double duality_gap = 0;
    while (true) {
      // centering step
      double totalresidualnorm = 0;
      while (true) {
        // compute the primal and dual Newton steps
        SchurcomputeNewtonStep(z, mu, t_kappa, delta_z, delta_mu,
                               totalresidualnorm);
        if (totalresidualnorm <= tolerance)
          break;
        else
          // backtracking line search for step length
          computeStepsizeAndNextState(z, mu, delta_z, delta_mu, t_kappa,
                                      totalresidualnorm);
      }
      // update the outer loop
      duality_gap = t_kappa * num_constraints;
      if (duality_gap < required_duality_gap) {
        delta_u = z.head(m);
        delta_alpha = z.segment(m, m);
        // warmstart(t_u, z.segment(m, n), z);
        warmstart(z, mu);
        break;
      } else
        t_kappa *= kappa_rate;
    }
  }
  // approximate primal-dual barrier method to solve QP in thruster allocation
  void approximateQPsolver_barrier(double t_kappa = 0.1) {
    int Kmax = 10;
    for (int i = 0; i != Kmax; ++i) {
      // centering step
      double totalresidualnorm = 0;
      while (true) {
        // compute the primal and dual Newton steps
        SchurcomputeNewtonStep(z, mu, t_kappa, delta_z, delta_mu,
                               totalresidualnorm);
        if (totalresidualnorm <= tolerance)
          break;
        else
          // backtracking line search for step length
          computeStepsizeAndNextState(z, mu, delta_z, delta_mu, t_kappa,
                                      totalresidualnorm);
      }
    }
    delta_u = z.head(m);
    delta_alpha = z.segment(m, m);
    // warmstart(t_u, z.segment(m, n), z);
    warmstart(z, mu);
  }
  // basic infeasible start primal barrier method to solve QP
  void basicQPsolver_interior(double t_kappa = 1) {
    double duality_gap = 0;
    double totalresidualnorm = 0;
    double max_dualprimalresidual = 0;
    while (true) {
      // compute the primal and dual Newton steps
      computeNewtonStep(z, mu, lambda, t_kappa, delta_z, delta_mu, delta_lambda,
                        totalresidualnorm, max_dualprimalresidual);
      // backtracking line search for step length
      computeStepsizeAndNextState(z, mu, lambda, delta_z, delta_mu,
                                  delta_lambda, t_kappa, totalresidualnorm);
      duality_gap = t_kappa * num_constraints;
      if (max_dualprimalresidual <= tolerance &&
          duality_gap < required_duality_gap) {
        delta_u = z.head(m);
        delta_alpha = z.segment(m, m);
        // warmstart(t_u, z.segment(m, n), z);
        warmstart(z, mu, lambda);
        break;
      } else
        t_kappa *= kappa_rate;
    }
  }
  // 1 order central difference
  double centraldifference(double fx_pdx, double fx_mdx, double dx) {
    return (fx_pdx - fx_mdx) / (2 * dx);
  }
  // 2 order central difference
  double secondorderdifference(double fx_pdx, double fx_mdx, double fx,
                               double dx) {
    return (fx_mdx + fx_pdx - 2 * fx) / (dx * dx);
  }
  // find elementwise max of two vectors
  Eigen::VectorXd findmax(const Eigen::VectorXd &va,
                          const Eigen::VectorXd &vb) {
    Eigen::VectorXd vc = Eigen::VectorXd::Zero(m);
    for (int i = 0; i != m; ++i) {
      vc(i) = std::max(va(i), vb(i));
    }
    return vc;
  }
  // find elementwise min of two vectors
  Eigen::VectorXd findmin(const Eigen::VectorXd &va,
                          const Eigen::VectorXd &vb) {
    Eigen::VectorXd vc = Eigen::VectorXd::Zero(m);
    for (int i = 0; i != m; ++i) {
      vc(i) = std::min(va(i), vb(i));
    }
    return vc;
  }
};

void output2csv(const pidcontrollerold &_pidcontroller,
                const std::string &_name);
void outputmatrices(const pidcontrollerold &_pidcontroller);

#endif /* _PIDCONTROLLEROLD_H_ */
