/*
***********************************************************************
* thrusterallocation.h:
* function for control allocation based on Quadratic programming, using
* Mosek solver API
* This header file can be read by C++ compilers
*
* by Hu.ZH(Mr.SJTU)
***********************************************************************
*/

#ifndef _THRUSTERALLOCATION_H_
#define _THRUSTERALLOCATION_H_
#include <math.h>
#include <stdio.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include "constants.h"
#include "mosek.h" /* Include the MOSEK definition file. */
#include "realtimedata.h"
#include "timecounter.hpp"

#define QP_THREADS_USED 2
// static void MSKAPI printstr(void *handle, const char str[]) {  //
// printf("%s", str);  // } /* printstr */

void write2csvfile(const std::string &, const Eigen::MatrixXd &);
void write2csvfile(const std::string &, const Eigen::MatrixXi &);

struct savefile {
  Eigen::MatrixXd save_u;
  Eigen::MatrixXd save_alpha;
  Eigen::MatrixXi save_alpha_deg;
  Eigen::MatrixXd save_Balphau;
  Eigen::MatrixXd save_tau;
  Eigen::MatrixXi save_rotation;
};

class thrusterallocation_first {
  friend void output2csv(const thrusterallocation_first &, const savefile &,
                         const std::string &);
  friend void outputmatrices(const thrusterallocation_first &);

 public:
  explicit thrusterallocation_first(const vessel_first &_vessel_first,
                                    realtimevessel_first &_realtimevessel)
      : m(_vessel_first.m),
        n(_vessel_first.n),
        numvar(_vessel_first.numvar),
        num_constraints(_vessel_first.num_constraints),
        Kbar_positive(_vessel_first.Kbar_positive),
        Kbar_negative(_vessel_first.Kbar_negative),
        max_delta_rotation_bow(_vessel_first.max_delta_rotation_bow),
        max_rotation_bow(_vessel_first.max_rotation_bow),
        max_thrust_bow_positive(_vessel_first.max_thrust_bow_positive),
        max_thrust_bow_negative(_vessel_first.max_thrust_bow_negative),
        K_left(_vessel_first.K_left),
        K_right(_vessel_first.K_right),
        max_delta_rotation_azimuth(_vessel_first.max_delta_rotation_azimuth),
        max_rotation_azimuth(_vessel_first.max_rotation_azimuth),
        max_thrust_azimuth_left(_vessel_first.max_thrust_azimuth_left),
        max_thrust_azimuth_right(_vessel_first.max_thrust_azimuth_right),
        max_delta_alpha_azimuth(_vessel_first.max_delta_alpha_azimuth),
        max_alpha_azimuth_left(_vessel_first.max_alpha_azimuth_left),
        min_alpha_azimuth_left(_vessel_first.min_alpha_azimuth_left),
        max_alpha_azimuth_right(_vessel_first.max_alpha_azimuth_right),
        min_alpha_azimuth_right(_vessel_first.min_alpha_azimuth_right),
        index_twice_bow(0),
        upper_delta_alpha_left(0),
        lower_delta_alpha_left(0),
        upper_delta_u_left(0),
        lower_delta_u_left(0),
        upper_delta_alpha_right(0),
        lower_delta_alpha_right(0),
        upper_delta_u_right(0),
        lower_delta_u_right(0),
        derivative_dx(1e-5),
        aptrb{0, 3, 6, 9, 12, 15, 18, 19, 20},
        aptre{3, 6, 9, 12, 15, 18, 19, 20, 21},
        asub{0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2},
        aval{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
        bkc{MSK_BK_FX, MSK_BK_FX, MSK_BK_FX},
        blc{0, 0, 0},
        buc{0, 0, 0},
        bkx{MSK_BK_RA, MSK_BK_RA, MSK_BK_RA, MSK_BK_RA, MSK_BK_RA,
            MSK_BK_RA, MSK_BK_FR, MSK_BK_FR, MSK_BK_FR},
        blx{0, 0, 0, 0, 0, 0, -MSK_INFINITY, -MSK_INFINITY, -MSK_INFINITY},
        bux{0, 0, 0, 0, 0, 0, +MSK_INFINITY, +MSK_INFINITY, +MSK_INFINITY},
        g{0, 0, 0, 0, 0, 0, 0, 0, 0},
        qsubi{0, 1, 2, 3, 4, 5, 6, 7, 8},
        qsubj{0, 1, 2, 3, 4, 5, 6, 7, 8},
        qval{0, 0, 0, 0, 0, 0, 0, 0, 0} {
    initializethrusterallocation(_vessel_first, _realtimevessel);
  }
  thrusterallocation_first() = delete;
  ~thrusterallocation_first() {
    MSK_deletetask(&task);
    MSK_deleteenv(&env);
  }

  // log file output
  void onestepthrusterallocation(realtimevessel_first &_realtimevessel,
                                 FILE *t_file = stdout) {
    updateTAparameters(_realtimevessel);

    int index = 0;
    if (index_twice_bow) {
      updateMosekparameters(0);
      onestepmosek(t_file, 0);
      updateMosekparameters(1);
      onestepmosek(t_file, 1);
      index = comparebowthruster();

    } else {
      updateMosekparameters();
      onestepmosek(t_file);
    }
    updateNextstep(_realtimevessel, index);
  }

  void test_multiplethrusterallocation(savefile &_savefile,
                                       const vessel_first &_vessel_first,
                                       realtimevessel_first &_realtimevessel) {
    // data saved for validation and viewer
    const int totalstep = 200;
    _savefile.save_u = Eigen::MatrixXd::Zero(_vessel_first.m, totalstep);
    _savefile.save_alpha = Eigen::MatrixXd::Zero(_vessel_first.m, totalstep);
    _savefile.save_alpha_deg =
        Eigen::MatrixXi::Zero(_vessel_first.m, totalstep);
    _savefile.save_Balphau = Eigen::MatrixXd::Zero(_vessel_first.n, totalstep);
    _savefile.save_tau = Eigen::MatrixXd::Zero(_vessel_first.n, totalstep);
    _savefile.save_rotation = Eigen::MatrixXi::Zero(_vessel_first.n, totalstep);

    // desired forces
    double angle = 0;
    for (int i = 0; i != 30; ++i) {
      angle = (i + 1) * M_PI / 15;
      _savefile.save_tau(2, i + 70) =
          0.0 * sin(angle) + 0.1 * std::rand() / RAND_MAX;
    }
    _savefile.save_tau.block(1, 0, 1, 100) =
        Eigen::MatrixXd::Constant(1, 100, 0.3) +
        0.0 * Eigen::MatrixXd::Random(1, 100);
    _savefile.save_tau.block(1, 100, 1, 100) =
        Eigen::MatrixXd::Constant(1, 100, 0.5) +
        0.0 * Eigen::MatrixXd::Random(1, 100);
    _savefile.save_tau.row(0) = 0.1 * Eigen::MatrixXd::Random(1, totalstep);
    for (int i = 0; i != totalstep; ++i) {
      // update tau
      _realtimevessel.tau = _savefile.save_tau.col(i);
      // thruster allocation
      onestepthrusterallocation(_realtimevessel);
      // save variables
      _savefile.save_u.col(i) = _realtimevessel.u;
      _savefile.save_alpha.col(i) = _realtimevessel.alpha;
      _savefile.save_alpha_deg.col(i) = _realtimevessel.alpha_deg;
      _savefile.save_Balphau.col(i) = _realtimevessel.BalphaU;
      _savefile.save_rotation.col(i) = _realtimevessel.rotation;
    }
  }

 private:
  // constants of the first vessel
  int m;
  int n;
  int numvar;
  int num_constraints;         // the number of constraints
  double Kbar_positive;        // alpha=90
  double Kbar_negative;        // alpha=-90
  int max_delta_rotation_bow;  // rpm
  int max_rotation_bow;        // rpm
  double max_thrust_bow_positive;
  double max_thrust_bow_negative;

  // relationship between rotation and thrust of azimuth thruster
  double K_left;
  double K_right;
  int max_delta_rotation_azimuth;  // rpm
  int max_rotation_azimuth;        // rpm
  double max_thrust_azimuth_left;
  double max_thrust_azimuth_right;
  double max_delta_alpha_azimuth;  // rad
  double max_alpha_azimuth_left;   // rad
  double min_alpha_azimuth_left;   // rad
  double max_alpha_azimuth_right;  // rad
  double min_alpha_azimuth_right;  // rad

  // real time constraints of bow thruster
  int index_twice_bow;
  Eigen::Vector2d upper_delta_alpha_bow;
  Eigen::Vector2d lower_delta_alpha_bow;
  Eigen::Vector2d upper_delta_u_bow;
  Eigen::Vector2d lower_delta_u_bow;
  // real time constraints on azimuth thruster
  double upper_delta_alpha_left;
  double lower_delta_alpha_left;
  double upper_delta_u_left;
  double lower_delta_u_left;
  double upper_delta_alpha_right;
  double lower_delta_alpha_right;
  double upper_delta_u_right;
  double lower_delta_u_right;

  // position of each thruster
  Eigen::Vector3d lx;
  Eigen::Vector3d ly;

  // quadratic objective
  Eigen::Matrix3d Q;
  Eigen::Matrix3d Omega;
  // real time Quadratic Objective in QP
  Eigen::Matrix3d Q_deltau;
  Eigen::Vector3d g_deltau;
  Eigen::Vector3d d_rho;  // derivative of rho term

  // real time constraint matrix in QP
  Eigen::Matrix3d B_alpha;
  Eigen::Matrix3d d_Balpha_u;  // Jocobian matrix of Balpha times u
  Eigen::Vector3d b;

  // real time physical variable in thruster allocation
  Eigen::Vector3d delta_alpha;  // rad
  Eigen::Vector3d delta_u;
  // linearized parameters
  double derivative_dx;  // step size of the derivative

  // parameters for Mosek API
  const MSKint32t aptrb[9], aptre[9], asub[21];
  double aval[21];
  /* Bounds on constraints. */
  const MSKboundkeye bkc[3];
  double blc[3];
  double buc[3];
  /* Bounds on variables. */
  const MSKboundkeye bkx[9];
  double blx[9];
  double bux[9];

  // objective g
  double g[9];

  // The lower triangular part of the quadratic objective Q matrix in the
  // objective is specified.
  const MSKint32t qsubi[9];
  const MSKint32t qsubj[9];
  double qval[9];
  // array to store the optimization results
  Eigen::Matrix<double, 9, 2> results;

  MSKenv_t env = NULL;
  MSKtask_t task = NULL;
  MSKrescodee r;

  void initializethrusterallocation(const vessel_first &_vessel_first,
                                    realtimevessel_first &_realtimevessel) {
    upper_delta_alpha_bow.setZero();
    lower_delta_alpha_bow.setZero();
    upper_delta_u_bow.setZero();
    lower_delta_u_bow.setZero();
    lx << _vessel_first.bow_x, _vessel_first.left_x, _vessel_first.right_x;
    ly << _vessel_first.bow_y, _vessel_first.left_y, _vessel_first.right_y;
    Q.setZero();
    Omega.setZero();
    Q_deltau.setZero();
    g_deltau.setZero();
    d_rho.setZero();
    B_alpha.setZero();
    d_Balpha_u.setZero();
    b.setZero();
    delta_alpha.setZero();
    delta_u.setZero();
    results.setZero();
    initializeQuadraticObjective();

    calculaterotation(_realtimevessel);
    // update BalphaU
    _realtimevessel.BalphaU =
        calculateBalphau(_realtimevessel.alpha, _realtimevessel.u);
    initializeMosekAPI(_vessel_first);
  }

  void initializeQuadraticObjective() {
    Q(0, 0) = 100;
    Q(1, 1) = 100;
    Q(2, 2) = 1000;
    Omega(0, 0) = 0.1;
    Omega(1, 1) = 1;
    Omega(2, 2) = 1;
    qval[3] = Omega(0, 0);
    qval[4] = Omega(1, 1);
    qval[5] = Omega(2, 2);
    qval[6] = Q(0, 0);
    qval[7] = Q(1, 1);
    qval[8] = Q(2, 2);
  }

  void initializeMosekAPI(const vessel_first &_vessel_first) {
    /* Create the mosek environment. */
    r = MSK_makeenv(&env, NULL);
    /* Create the optimization task. */
    r = MSK_maketask(env, _vessel_first.num_constraints, _vessel_first.numvar,
                     &task);
    // set up the threads used by mosek
    r = MSK_putintparam(task, MSK_IPAR_NUM_THREADS, QP_THREADS_USED);
    // r = MSK_linkfunctotaskstream(task, MSK_STREAM_LOG, NULL, printstr);
    r = MSK_linkfunctotaskstream(task, MSK_STREAM_LOG, NULL, NULL);
    // append num_constrainsts empty contraints
    r = MSK_appendcons(task, _vessel_first.num_constraints);
    // append numvar emtpy variables
    r = MSK_appendvars(task, _vessel_first.numvar);
  }
  // calculate the contraints of bow thruster
  void calculateconstrains_bowthruster(
      const realtimevessel_first &_realtimevessel) {
    if (0 < _realtimevessel.rotation(0) &&
        _realtimevessel.rotation(0) <= max_delta_rotation_bow) {
      // specify the first case
      upper_delta_alpha_bow(0) = 0;
      lower_delta_alpha_bow(0) = 0;
      lower_delta_u_bow(0) = -_realtimevessel.u(0);
      upper_delta_u_bow(0) =
          Kbar_positive *
              (_realtimevessel.rotation(0) + max_delta_rotation_bow) *
              (_realtimevessel.rotation(0) + max_delta_rotation_bow) -
          _realtimevessel.u(0);
      upper_delta_alpha_bow(1) = -M_PI;
      lower_delta_alpha_bow(1) = -M_PI;
      lower_delta_u_bow(1) = -_realtimevessel.u(0);
      upper_delta_u_bow(1) =
          Kbar_negative *
              (_realtimevessel.rotation(0) - max_delta_rotation_bow) *
              (_realtimevessel.rotation(0) - max_delta_rotation_bow) -
          _realtimevessel.u(0);

      // specify whether two optimization problems are solved or not
      index_twice_bow = 1;
    } else if (-max_delta_rotation_bow <= _realtimevessel.rotation(0) &&
               _realtimevessel.rotation(0) < 0) {
      // specify the first case
      upper_delta_alpha_bow(0) = 0;
      lower_delta_alpha_bow(0) = 0;
      lower_delta_u_bow(0) = -_realtimevessel.u(0);
      upper_delta_u_bow(0) =
          Kbar_negative *
              (_realtimevessel.rotation(0) - max_delta_rotation_bow) *
              (_realtimevessel.rotation(0) - max_delta_rotation_bow) -
          _realtimevessel.u(0);
      // specify the second case
      upper_delta_alpha_bow(1) = M_PI;
      lower_delta_alpha_bow(1) = M_PI;
      lower_delta_u_bow(1) = -_realtimevessel.u(0);
      upper_delta_u_bow(1) =
          Kbar_positive *
              (_realtimevessel.rotation(0) + max_delta_rotation_bow) *
              (_realtimevessel.rotation(0) + max_delta_rotation_bow) -
          _realtimevessel.u(0);
      // specify whether two optimization problems are solved or not
      index_twice_bow = 1;
    } else if (_realtimevessel.rotation(0) > max_delta_rotation_bow) {
      lower_delta_alpha_bow(0) = 0;
      upper_delta_alpha_bow(0) = 0;
      upper_delta_u_bow(0) = std::min(
          max_thrust_bow_positive - _realtimevessel.u(0),
          Kbar_positive *
                  (_realtimevessel.rotation(0) + max_delta_rotation_bow) *
                  (_realtimevessel.rotation(0) + max_delta_rotation_bow) -
              _realtimevessel.u(0));
      lower_delta_u_bow(0) =
          Kbar_positive *
              (_realtimevessel.rotation(0) - max_delta_rotation_bow) *
              (_realtimevessel.rotation(0) - max_delta_rotation_bow) -
          _realtimevessel.u(0);
      // specify whether two optimization problems are solved or not
      index_twice_bow = 0;
    } else {
      lower_delta_alpha_bow(0) = 0;
      upper_delta_alpha_bow(0) = 0;
      upper_delta_u_bow(0) = std::min(
          Kbar_negative *
                  (_realtimevessel.rotation(0) - max_delta_rotation_bow) *
                  (_realtimevessel.rotation(0) - max_delta_rotation_bow) -
              _realtimevessel.u(0),
          max_thrust_bow_negative - _realtimevessel.u(0));
      lower_delta_u_bow(0) =
          Kbar_negative *
              (_realtimevessel.rotation(0) + max_delta_rotation_bow) *
              (_realtimevessel.rotation(0) + max_delta_rotation_bow) -
          _realtimevessel.u(0);
      // specify whether two optimization problems are solved or not
      index_twice_bow = 0;
    }
  }
  // calculate the consraints of azimuth thruster
  void calculateconstrains_azimuth(
      const realtimevessel_first &_realtimevessel) {
    // specify constriants on left azimuth
    upper_delta_alpha_left =
        std::min(max_delta_alpha_azimuth,
                 max_alpha_azimuth_left - _realtimevessel.alpha(1));
    lower_delta_alpha_left =
        std::max(-max_delta_alpha_azimuth,
                 min_alpha_azimuth_left - _realtimevessel.alpha(1));

    double thrust_azimuth_left =
        K_left * _realtimevessel.rotation(1) * _realtimevessel.rotation(1);
    upper_delta_u_left = std::min(
        K_left * (_realtimevessel.rotation(1) + max_delta_rotation_azimuth) *
                (_realtimevessel.rotation(1) + max_delta_rotation_azimuth) -
            thrust_azimuth_left,
        max_thrust_azimuth_left - thrust_azimuth_left);
    if (_realtimevessel.rotation(1) <= max_delta_rotation_azimuth)
      lower_delta_u_left = -thrust_azimuth_left;
    else
      lower_delta_u_left =
          K_left * (_realtimevessel.rotation(1) - max_delta_rotation_azimuth) *
              (_realtimevessel.rotation(1) - max_delta_rotation_azimuth) -
          thrust_azimuth_left;

    // specify constraints on right azimuth
    upper_delta_alpha_right =
        std::min(max_delta_alpha_azimuth,
                 max_alpha_azimuth_right - _realtimevessel.alpha(2));
    lower_delta_alpha_right =
        std::max(-max_delta_alpha_azimuth,
                 min_alpha_azimuth_right - _realtimevessel.alpha(2));
    double thrust_azimuth_right =
        K_right * _realtimevessel.rotation(2) * _realtimevessel.rotation(2);
    upper_delta_u_right = std::min(
        K_right * (_realtimevessel.rotation(2) + max_delta_rotation_azimuth) *
                (_realtimevessel.rotation(2) + max_delta_rotation_azimuth) -
            thrust_azimuth_right,
        max_thrust_azimuth_right - thrust_azimuth_right);
    if (_realtimevessel.rotation(2) <= max_delta_rotation_azimuth)
      lower_delta_u_right = -thrust_azimuth_right;
    else
      lower_delta_u_right =
          K_right * (_realtimevessel.rotation(2) - max_delta_rotation_azimuth) *
              (_realtimevessel.rotation(2) - max_delta_rotation_azimuth) -
          thrust_azimuth_right;
  }

  // calculate Balpha as function of alpha
  Eigen::Matrix3d calculateBalpha(const Eigen::Vector3d &t_alpha) {
    Eigen::Matrix3d _B_alpha = Eigen::Matrix3d::Zero();
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
  double calculateRhoTerm(const Eigen::Vector3d &t_alpha, double epsilon = 0.01,
                          double rho = 10) {
    auto _B_alpha = calculateBalpha(t_alpha);
    Eigen::Matrix3d BBT = _B_alpha * _B_alpha.transpose();
    return rho / (epsilon + BBT.determinant());
  }
  // calculate Jacobian using central difference
  void calculateJocobianRhoTerm(const Eigen::Vector3d &t_alpha) {
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
  Eigen::Vector3d calculateBalphau(const Eigen::Vector3d &t_alpha,
                                   const Eigen::Vector3d &t_u) {
    return calculateBalpha(t_alpha) * t_u;
  }
  // calculate derivative of Balpha times u
  void calculateJocobianBalphaU(const Eigen::Vector3d &t_alpha,
                                const Eigen::Vector3d &t_u) {
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
  void calculateDeltauQ(const Eigen::Vector3d &t_u) {
    Eigen::Vector3d d_utemp = Eigen::Vector3d::Zero();
    d_utemp = t_u.cwiseSqrt();
    g_deltau = 1.5 * d_utemp;
    Eigen::Vector3d Q_temp = Eigen::Vector3d::Zero();
    Q_temp = 0.75 * d_utemp.cwiseInverse();
    Q_deltau = Q_temp.asDiagonal();
  }
  // calculate the BalphaU and b
  void calculateb(const realtimevessel_first &_realtimevessel) {
    b = _realtimevessel.tau - _realtimevessel.BalphaU;
  }
  // update parameters in thruster allocation for each time step
  void updateTAparameters(const realtimevessel_first &_realtimevessel) {
    B_alpha = calculateBalpha(_realtimevessel.alpha);
    calculateJocobianRhoTerm(_realtimevessel.alpha);
    calculateJocobianBalphaU(_realtimevessel.alpha, _realtimevessel.u);
    calculateDeltauQ(_realtimevessel.u);
    calculateb(_realtimevessel);
    calculateconstrains_bowthruster(_realtimevessel);
    calculateconstrains_azimuth(_realtimevessel);
  }

  void updateMosekparameters(int index = 0) {
    // update A values
    aval[0] = B_alpha(0, 0);
    aval[1] = B_alpha(1, 0);
    aval[2] = B_alpha(2, 0);
    aval[3] = B_alpha(0, 1);
    aval[4] = B_alpha(1, 1);
    aval[5] = B_alpha(2, 1);
    aval[6] = B_alpha(0, 2);
    aval[7] = B_alpha(1, 2);
    aval[8] = B_alpha(2, 2);
    aval[9] = d_Balpha_u(0, 0);
    aval[10] = d_Balpha_u(1, 0);
    aval[11] = d_Balpha_u(2, 0);
    aval[12] = d_Balpha_u(0, 1);
    aval[13] = d_Balpha_u(1, 1);
    aval[14] = d_Balpha_u(2, 1);
    aval[15] = d_Balpha_u(0, 2);
    aval[16] = d_Balpha_u(1, 2);
    aval[17] = d_Balpha_u(2, 2);

    // update linear constraints
    for (int i = 0; i != num_constraints; ++i) {
      blc[i] = b(i);
      buc[i] = b(i);
    }
    // update variable constraints

    blx[0] = lower_delta_u_bow[index];
    blx[1] = lower_delta_u_left;
    blx[2] = lower_delta_u_right;
    blx[3] = lower_delta_alpha_bow[index];
    blx[4] = lower_delta_alpha_left;
    blx[5] = lower_delta_alpha_right;

    bux[0] = upper_delta_u_bow[index];
    bux[1] = upper_delta_u_left;
    bux[2] = upper_delta_u_right;
    bux[3] = upper_delta_alpha_bow[index];
    bux[4] = upper_delta_alpha_left;
    bux[5] = upper_delta_alpha_right;

    // update objective g
    g[0] = g_deltau(0);
    g[1] = g_deltau(1);
    g[2] = g_deltau(2);
    g[3] = d_rho(0);
    g[4] = d_rho(1);
    g[5] = d_rho(2);

    // The lower triangular part of the quadratic objective Q matrix in the
    // objective is specified.
    for (int i = 0; i != n; ++i) qval[i] = Q_deltau(i, i);
  }

  // update parameters in QP for each time step
  void onestepmosek(FILE *t_file, int index = 0) {
    MSKint32t i, j;
    double t_results[numvar];

    if (r == MSK_RES_OK) {
      for (j = 0; j < numvar; ++j) {
        /* Set the linear term g_j in the objective.*/
        r = MSK_putcj(task, j, g[j]);

        /* Set the bounds on variable j.
         blx[j] <= x_j <= bux[j] */

        r = MSK_putvarbound(task, j, /* Index of variable.*/
                            bkx[j],  /* Bound key.*/
                            blx[j],  /* Numerical value of lower bound.*/
                            bux[j]); /* Numerical value of upper bound.*/

        /* Input column j of A */
        r = MSK_putacol(
            task, j,             /* Variable (column) index.*/
            aptre[j] - aptrb[j], /* Number of non-zeros in column j.*/
            asub + aptrb[j],     /* Pointer to row indexes of column j.*/
            aval + aptrb[j]);    /* Pointer to Values of column j.*/
      }

      /* Set the bounds on constraints.
         for i=1, ...,NUMCON : blc[i] <= constraint i <= buc[i] */
      for (i = 0; i < num_constraints; ++i)
        r = MSK_putconbound(task, i, /* Index of constraint.*/
                            bkc[i],  /* Bound key.*/
                            blc[i],  /* Numerical value of lower bound.*/
                            buc[i]); /* Numerical value of upper bound.*/

      /* Input the Q for the objective. */
      r = MSK_putqobj(task, 9, qsubi, qsubj, qval);

      if (r == MSK_RES_OK) {
        MSKrescodee trmcode;

        /* Run optimizer */
        r = MSK_optimizetrm(task, &trmcode);

        /* Print a summary containing information
           about the solution for debugging purposes*/
        // MSK_solutionsummary(task, MSK_STREAM_MSG);

        if (r == MSK_RES_OK) {
          MSKsolstae solsta;
          int j;

          MSK_getsolsta(task, MSK_SOL_ITR, &solsta);

          switch (solsta) {
            case MSK_SOL_STA_OPTIMAL:
            case MSK_SOL_STA_NEAR_OPTIMAL:
              MSK_getxx(task, MSK_SOL_ITR, /* Request the interior solution. */
                        t_results);

              for (j = 0; j < numvar; ++j) {
                results(j, index) = t_results[j];
              }

              break;

            case MSK_SOL_STA_DUAL_INFEAS_CER:
            case MSK_SOL_STA_PRIM_INFEAS_CER:
            case MSK_SOL_STA_NEAR_DUAL_INFEAS_CER:
            case MSK_SOL_STA_NEAR_PRIM_INFEAS_CER: {
              t_file = fopen(logsavepath.c_str(), "a+");
              fprintf(
                  t_file,
                  "First: Primal or dual infeasibility certificate found.\n");
              fclose(t_file);
              break;
            }
            case MSK_SOL_STA_UNKNOWN: {
              t_file = fopen(logsavepath.c_str(), "a+");
              fprintf(t_file,
                      "First: The status of the solution could not be "
                      "determined.\n");
              fclose(t_file);
              break;
            }
            default: {
              t_file = fopen(logsavepath.c_str(), "a+");
              fprintf(t_file, "First: Other solution status.");
              fclose(t_file);
              break;
            }
          }
        } else {
          t_file = fopen(logsavepath.c_str(), "a+");
          fprintf(t_file, "First: Error while optimizing.\n");
          fclose(t_file);
        }
      }
      if (r != MSK_RES_OK) {
        /* In case of an error print error code and description. */
        char symname[MSK_MAX_STR_LEN];
        char desc[MSK_MAX_STR_LEN];
        t_file = fopen(logsavepath.c_str(), "a+");
        fprintf(t_file, "First: An error occurred while optimizing.\n");
        MSK_getcodedesc(r, symname, desc);
        fprintf(t_file, "Error %s - '%s'\n", symname, desc);
        fclose(t_file);
      }
    }
  }
  // compare two scenario for bow thruster
  int comparebowthruster() {
    Eigen::Vector3d S1 = results.block(2 * m, 0, n, 1);
    Eigen::Vector3d S2 = results.block(2 * m, 1, n, 1);

    double position_residual1 = S1.transpose() * (Q * S1);
    double position_residual2 = S2.transpose() * (Q * S2);

    if (position_residual1 < position_residual2)
      return 0;
    else
      return 1;
  }

  // calculate vessel parameters at the next time step
  void updateNextstep(realtimevessel_first &_realtimevessel, int index) {
    // calculate delta variable using Mosek results
    delta_u = results.block(0, index, m, 1);
    delta_alpha = results.block(m, index, m, 1);

    // update alpha and u
    updateAlphaandU(_realtimevessel);
    // convert the double alpha(rad) to int alpha(deg)
    convertdoublealpha2int(_realtimevessel);
    // update rotation speed
    calculaterotation(_realtimevessel);
    // update BalphaU
    _realtimevessel.BalphaU =
        calculateBalphau(_realtimevessel.alpha, _realtimevessel.u);
  }

  // update alpha and u using computed delta_alpha and delta_u
  void updateAlphaandU(realtimevessel_first &_realtimevessel) {
    _realtimevessel.u += delta_u;
    _realtimevessel.alpha += delta_alpha;
  }
  // the precision of alpha of each azimuth thruster is 1 deg
  void convertdoublealpha2int(realtimevessel_first &_realtimevessel) {
    for (int i = 0; i != m; ++i) {
      _realtimevessel.alpha_deg(i) =
          (int)(_realtimevessel.alpha(i) * 180 / M_PI);  // round to int (deg)
      _realtimevessel.alpha(i) = _realtimevessel.alpha_deg(i) * M_PI / 180;
    }
  }
  // calcuate rotation speed of each thruster based on thrust
  void calculaterotation(realtimevessel_first &_realtimevessel) {
    int t_rotation = 0;
    // bow thruster
    if (_realtimevessel.alpha(0) < 0) {
      t_rotation = (int)(sqrt(abs(_realtimevessel.u(0)) / Kbar_negative));
      if (t_rotation == 0) {
        _realtimevessel.rotation(0) = -1;  // prevent zero
        _realtimevessel.u(0) = Kbar_negative;
      } else
        _realtimevessel.rotation(0) = -t_rotation;

    } else {
      t_rotation = (int)(sqrt(abs(_realtimevessel.u(0)) / Kbar_positive));
      if (t_rotation == 0) {
        _realtimevessel.rotation(0) = 1;  // prevent zero
        _realtimevessel.u(0) = Kbar_positive;
      } else
        _realtimevessel.rotation(0) = t_rotation;
    }
    // azimuth thruster Left
    t_rotation = (int)(sqrt(abs(_realtimevessel.u(1)) / K_left));
    if (t_rotation == 0) {
      _realtimevessel.rotation(1) = 1;
      _realtimevessel.u(1) = K_left;
    } else
      _realtimevessel.rotation(1) = t_rotation;
    // azimuth thruster Right
    t_rotation = (int)(sqrt(abs(_realtimevessel.u(2)) / K_right));
    if (t_rotation == 0) {
      _realtimevessel.rotation(2) = 1;
      _realtimevessel.u(2) = K_right;
    } else
      _realtimevessel.rotation(2) = t_rotation;
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
};

class thrusterallocation_second {
  friend void output2csv(const thrusterallocation_second &, const savefile &,
                         const std::string &);
  friend void outputmatrices(const thrusterallocation_second &);

 public:
  explicit thrusterallocation_second(const vessel_second &_vessel_second,
                                     realtimevessel_second &_realtimevessel)
      : m(_vessel_second.m),
        n(_vessel_second.n),
        numvar(_vessel_second.numvar),
        num_constraints(_vessel_second.num_constraints),
        Kbar_positive(_vessel_second.Kbar_positive),
        Kbar_negative(_vessel_second.Kbar_negative),
        max_delta_rotation_bow(_vessel_second.max_delta_rotation_bow),
        max_rotation_bow(_vessel_second.max_rotation_bow),
        max_thrust_bow_positive(_vessel_second.max_thrust_bow_positive),
        max_thrust_bow_negative(_vessel_second.max_thrust_bow_negative),
        K_left(_vessel_second.K_left),
        K_right(_vessel_second.K_right),
        max_delta_rotation_azimuth(_vessel_second.max_delta_rotation_azimuth),
        max_rotation_azimuth(_vessel_second.max_rotation_azimuth),
        max_thrust_azimuth_left(_vessel_second.max_thrust_azimuth_left),
        max_thrust_azimuth_right(_vessel_second.max_thrust_azimuth_right),
        max_delta_alpha_azimuth(_vessel_second.max_delta_alpha_azimuth),
        max_alpha_azimuth_left(_vessel_second.max_alpha_azimuth_left),
        min_alpha_azimuth_left(_vessel_second.min_alpha_azimuth_left),
        max_alpha_azimuth_right(_vessel_second.max_alpha_azimuth_right),
        min_alpha_azimuth_right(_vessel_second.min_alpha_azimuth_right),
        index_twice_bow(0),
        upper_delta_alpha_left(0),
        lower_delta_alpha_left(0),
        upper_delta_u_left(0),
        lower_delta_u_left(0),
        upper_delta_alpha_right(0),
        lower_delta_alpha_right(0),
        upper_delta_u_right(0),
        lower_delta_u_right(0),
        derivative_dx(1e-5),
        aptrb{0, 3, 6, 9, 12, 15, 18, 19, 20},
        aptre{3, 6, 9, 12, 15, 18, 19, 20, 21},
        asub{0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2},
        aval{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
        bkc{MSK_BK_FX, MSK_BK_FX, MSK_BK_FX},
        blc{0, 0, 0},
        buc{0, 0, 0},
        bkx{MSK_BK_RA, MSK_BK_RA, MSK_BK_RA, MSK_BK_RA, MSK_BK_RA,
            MSK_BK_RA, MSK_BK_FR, MSK_BK_FR, MSK_BK_FR},
        blx{0, 0, 0, 0, 0, 0, -MSK_INFINITY, -MSK_INFINITY, -MSK_INFINITY},
        bux{0, 0, 0, 0, 0, 0, +MSK_INFINITY, +MSK_INFINITY, +MSK_INFINITY},
        g{0, 0, 0, 0, 0, 0, 0, 0, 0},
        qsubi{0, 1, 2, 3, 4, 5, 6, 7, 8},
        qsubj{0, 1, 2, 3, 4, 5, 6, 7, 8},
        qval{0, 0, 0, 0, 0, 0, 0, 0, 0} {
    initializethrusterallocation(_vessel_second, _realtimevessel);
  }
  thrusterallocation_second() = delete;
  ~thrusterallocation_second() {
    MSK_deletetask(&task);
    MSK_deleteenv(&env);
  }

  void onestepthrusterallocation(realtimevessel_second &_realtimevessel,
                                 FILE *t_file = stdout) {
    updateTAparameters(_realtimevessel);

    int index = 0;
    if (index_twice_bow) {
      updateMosekparameters(0);
      onestepmosek(t_file, 0);
      updateMosekparameters(1);
      onestepmosek(t_file, 1);
      index = comparebowthruster();

    } else {
      updateMosekparameters();
      onestepmosek(t_file);
    }
    updateNextstep(_realtimevessel, index);
  }

  void test_multiplethrusterallocation(savefile &_savefile,
                                       const vessel_second &_vessel_second,
                                       realtimevessel_second &_realtimevessel) {
    // data saved for validation and viewer
    const int totalstep = 200;
    _savefile.save_u = Eigen::MatrixXd::Zero(_vessel_second.m, totalstep);
    _savefile.save_alpha = Eigen::MatrixXd::Zero(_vessel_second.m, totalstep);
    _savefile.save_alpha_deg =
        Eigen::MatrixXi::Zero(_vessel_second.m, totalstep);
    _savefile.save_Balphau = Eigen::MatrixXd::Zero(_vessel_second.n, totalstep);
    _savefile.save_tau = Eigen::MatrixXd::Zero(_vessel_second.n, totalstep);
    _savefile.save_rotation =
        Eigen::MatrixXi::Zero(_vessel_second.n, totalstep);

    // desired forces
    double angle = 0;
    for (int i = 0; i != 30; ++i) {
      angle = (i + 1) * M_PI / 15;
      _savefile.save_tau(2, i + 70) =
          0.0 * sin(angle) + 0.1 * std::rand() / RAND_MAX;
    }
    _savefile.save_tau.block(1, 0, 1, 100) =
        Eigen::MatrixXd::Constant(1, 100, 0.3) +
        0.0 * Eigen::MatrixXd::Random(1, 100);
    _savefile.save_tau.block(1, 100, 1, 100) =
        Eigen::MatrixXd::Constant(1, 100, 0.5) +
        0.0 * Eigen::MatrixXd::Random(1, 100);
    _savefile.save_tau.row(0) = 0.1 * Eigen::MatrixXd::Random(1, totalstep);
    for (int i = 0; i != totalstep; ++i) {
      // update tau
      _realtimevessel.tau = _savefile.save_tau.col(i);
      // thruster allocation
      onestepthrusterallocation(_realtimevessel);
      // save variables
      _savefile.save_u.col(i) = _realtimevessel.u;
      _savefile.save_alpha.col(i) = _realtimevessel.alpha;
      _savefile.save_alpha_deg.col(i) = _realtimevessel.alpha_deg;
      _savefile.save_Balphau.col(i) = _realtimevessel.BalphaU;
      _savefile.save_rotation.col(i) = _realtimevessel.rotation;
    }
  }

 private:
  // constants of the first vessel
  int m;
  int n;
  int numvar;
  int num_constraints;         // the number of constraints
  double Kbar_positive;        // alpha=90
  double Kbar_negative;        // alpha=-90
  int max_delta_rotation_bow;  // rpm
  int max_rotation_bow;        // rpm
  double max_thrust_bow_positive;
  double max_thrust_bow_negative;

  // relationship between rotation and thrust of azimuth thruster
  double K_left;
  double K_right;
  int max_delta_rotation_azimuth;  // rpm
  int max_rotation_azimuth;        // rpm
  double max_thrust_azimuth_left;
  double max_thrust_azimuth_right;
  double max_delta_alpha_azimuth;  // rad
  double max_alpha_azimuth_left;   // rad
  double min_alpha_azimuth_left;   // rad
  double max_alpha_azimuth_right;  // rad
  double min_alpha_azimuth_right;  // rad

  // real time constraints of bow thruster
  int index_twice_bow;
  Eigen::Vector2d upper_delta_alpha_bow;
  Eigen::Vector2d lower_delta_alpha_bow;
  Eigen::Vector2d upper_delta_u_bow;
  Eigen::Vector2d lower_delta_u_bow;
  // real time constraints on azimuth thruster
  double upper_delta_alpha_left;
  double lower_delta_alpha_left;
  double upper_delta_u_left;
  double lower_delta_u_left;
  double upper_delta_alpha_right;
  double lower_delta_alpha_right;
  double upper_delta_u_right;
  double lower_delta_u_right;

  // position of each thruster
  Eigen::Vector3d lx;
  Eigen::Vector3d ly;

  // quadratic objective
  Eigen::Matrix3d Q;
  Eigen::Matrix3d Omega;
  // real time Quadratic Objective in QP
  Eigen::Matrix3d Q_deltau;
  Eigen::Vector3d g_deltau;
  Eigen::Vector3d d_rho;  // derivative of rho term

  // real time constraint matrix in QP
  Eigen::Matrix3d B_alpha;
  Eigen::Matrix3d d_Balpha_u;  // Jocobian matrix of Balpha times u
  Eigen::Vector3d b;

  // real time physical variable in thruster allocation
  Eigen::Vector3d delta_alpha;  // rad
  Eigen::Vector3d delta_u;
  // linearized parameters
  double derivative_dx;  // step size of the derivative

  // parameters for Mosek API
  const MSKint32t aptrb[9], aptre[9], asub[21];
  double aval[21];
  /* Bounds on constraints. */
  const MSKboundkeye bkc[3];
  double blc[3];
  double buc[3];
  /* Bounds on variables. */
  const MSKboundkeye bkx[9];
  double blx[9];
  double bux[9];

  // objective g
  double g[9];

  // The lower triangular part of the quadratic objective Q matrix in the
  // objective is specified.
  const MSKint32t qsubi[9];
  const MSKint32t qsubj[9];
  double qval[9];
  // array to store the optimization results
  Eigen::Matrix<double, 9, 2> results;

  MSKenv_t env = NULL;
  MSKtask_t task = NULL;
  MSKrescodee r;

  void initializethrusterallocation(const vessel_second &_vessel_second,
                                    realtimevessel_second &_realtimevessel) {
    upper_delta_alpha_bow.setZero();
    lower_delta_alpha_bow.setZero();
    upper_delta_u_bow.setZero();
    lower_delta_u_bow.setZero();
    lx << _vessel_second.bow_x, _vessel_second.left_x, _vessel_second.right_x;
    ly << _vessel_second.bow_y, _vessel_second.left_y, _vessel_second.right_y;
    Q.setZero();
    Omega.setZero();
    Q_deltau.setZero();
    g_deltau.setZero();
    d_rho.setZero();
    B_alpha.setZero();
    d_Balpha_u.setZero();
    b.setZero();
    delta_alpha.setZero();
    delta_u.setZero();
    results.setZero();
    initializeQuadraticObjective();

    calculaterotation(_realtimevessel);
    // update BalphaU
    _realtimevessel.BalphaU =
        calculateBalphau(_realtimevessel.alpha, _realtimevessel.u);
    initializeMosekAPI(_vessel_second);
  }

  void initializeQuadraticObjective() {
    Q(0, 0) = 100;
    Q(1, 1) = 100;
    Q(2, 2) = 100;
    Omega(0, 0) = 0.1;
    Omega(1, 1) = 1;
    Omega(2, 2) = 1;
    qval[3] = Omega(0, 0);
    qval[4] = Omega(1, 1);
    qval[5] = Omega(2, 2);
    qval[6] = Q(0, 0);
    qval[7] = Q(1, 1);
    qval[8] = Q(2, 2);
  }

  void initializeMosekAPI(const vessel_second &_vessel_second) {
    /* Create the mosek environment. */
    r = MSK_makeenv(&env, NULL);
    /* Create the optimization task. */
    r = MSK_maketask(env, _vessel_second.num_constraints, _vessel_second.numvar,
                     &task);
    // set up the threads used by mosek
    r = MSK_putintparam(task, MSK_IPAR_NUM_THREADS, QP_THREADS_USED);
    // r = MSK_linkfunctotaskstream(task, MSK_STREAM_LOG, NULL, printstr);
    r = MSK_linkfunctotaskstream(task, MSK_STREAM_LOG, NULL, NULL);
    // append num_constrainsts empty contraints
    r = MSK_appendcons(task, _vessel_second.num_constraints);
    // append numvar emtpy variables
    r = MSK_appendvars(task, _vessel_second.numvar);
  }
  // calculate the contraints of bow thruster
  void calculateconstrains_bowthruster(
      const realtimevessel_second &_realtimevessel) {
    if (0 < _realtimevessel.rotation(0) &&
        _realtimevessel.rotation(0) <= max_delta_rotation_bow) {
      // specify the first case
      upper_delta_alpha_bow(0) = 0;
      lower_delta_alpha_bow(0) = 0;
      lower_delta_u_bow(0) = -_realtimevessel.u(0);
      upper_delta_u_bow(0) =
          Kbar_positive *
              (_realtimevessel.rotation(0) + max_delta_rotation_bow) *
              (_realtimevessel.rotation(0) + max_delta_rotation_bow) -
          _realtimevessel.u(0);
      upper_delta_alpha_bow(1) = -M_PI;
      lower_delta_alpha_bow(1) = -M_PI;
      lower_delta_u_bow(1) = -_realtimevessel.u(0);
      upper_delta_u_bow(1) =
          Kbar_negative *
              (_realtimevessel.rotation(0) - max_delta_rotation_bow) *
              (_realtimevessel.rotation(0) - max_delta_rotation_bow) -
          _realtimevessel.u(0);

      // specify whether two optimization problems are solved or not
      index_twice_bow = 1;
    } else if (-max_delta_rotation_bow <= _realtimevessel.rotation(0) &&
               _realtimevessel.rotation(0) < 0) {
      // specify the first case
      upper_delta_alpha_bow(0) = 0;
      lower_delta_alpha_bow(0) = 0;
      lower_delta_u_bow(0) = -_realtimevessel.u(0);
      upper_delta_u_bow(0) =
          Kbar_negative *
              (_realtimevessel.rotation(0) - max_delta_rotation_bow) *
              (_realtimevessel.rotation(0) - max_delta_rotation_bow) -
          _realtimevessel.u(0);
      // specify the second case
      upper_delta_alpha_bow(1) = M_PI;
      lower_delta_alpha_bow(1) = M_PI;
      lower_delta_u_bow(1) = -_realtimevessel.u(0);
      upper_delta_u_bow(1) =
          Kbar_positive *
              (_realtimevessel.rotation(0) + max_delta_rotation_bow) *
              (_realtimevessel.rotation(0) + max_delta_rotation_bow) -
          _realtimevessel.u(0);
      // specify whether two optimization problems are solved or not
      index_twice_bow = 1;
    } else if (_realtimevessel.rotation(0) > max_delta_rotation_bow) {
      lower_delta_alpha_bow(0) = 0;
      upper_delta_alpha_bow(0) = 0;
      upper_delta_u_bow(0) = std::min(
          max_thrust_bow_positive - _realtimevessel.u(0),
          Kbar_positive *
                  (_realtimevessel.rotation(0) + max_delta_rotation_bow) *
                  (_realtimevessel.rotation(0) + max_delta_rotation_bow) -
              _realtimevessel.u(0));
      lower_delta_u_bow(0) =
          Kbar_positive *
              (_realtimevessel.rotation(0) - max_delta_rotation_bow) *
              (_realtimevessel.rotation(0) - max_delta_rotation_bow) -
          _realtimevessel.u(0);
      // specify whether two optimization problems are solved or not
      index_twice_bow = 0;
    } else {
      lower_delta_alpha_bow(0) = 0;
      upper_delta_alpha_bow(0) = 0;
      upper_delta_u_bow(0) = std::min(
          Kbar_negative *
                  (_realtimevessel.rotation(0) - max_delta_rotation_bow) *
                  (_realtimevessel.rotation(0) - max_delta_rotation_bow) -
              _realtimevessel.u(0),
          max_thrust_bow_negative - _realtimevessel.u(0));
      lower_delta_u_bow(0) =
          Kbar_negative *
              (_realtimevessel.rotation(0) + max_delta_rotation_bow) *
              (_realtimevessel.rotation(0) + max_delta_rotation_bow) -
          _realtimevessel.u(0);
      // specify whether two optimization problems are solved or not
      index_twice_bow = 0;
    }
  }
  // calculate the consraints of azimuth thruster
  void calculateconstrains_azimuth(
      const realtimevessel_second &_realtimevessel) {
    // specify constriants on left azimuth
    upper_delta_alpha_left =
        std::min(max_delta_alpha_azimuth,
                 max_alpha_azimuth_left - _realtimevessel.alpha(1));
    lower_delta_alpha_left =
        std::max(-max_delta_alpha_azimuth,
                 min_alpha_azimuth_left - _realtimevessel.alpha(1));

    double thrust_azimuth_left =
        K_left * _realtimevessel.rotation(1) * _realtimevessel.rotation(1);
    upper_delta_u_left = std::min(
        K_left * (_realtimevessel.rotation(1) + max_delta_rotation_azimuth) *
                (_realtimevessel.rotation(1) + max_delta_rotation_azimuth) -
            thrust_azimuth_left,
        max_thrust_azimuth_left - thrust_azimuth_left);
    if (_realtimevessel.rotation(1) <= max_delta_rotation_azimuth)
      lower_delta_u_left = -thrust_azimuth_left;
    else
      lower_delta_u_left =
          K_left * (_realtimevessel.rotation(1) - max_delta_rotation_azimuth) *
              (_realtimevessel.rotation(1) - max_delta_rotation_azimuth) -
          thrust_azimuth_left;

    // specify constraints on right azimuth
    upper_delta_alpha_right =
        std::min(max_delta_alpha_azimuth,
                 max_alpha_azimuth_right - _realtimevessel.alpha(2));
    lower_delta_alpha_right =
        std::max(-max_delta_alpha_azimuth,
                 min_alpha_azimuth_right - _realtimevessel.alpha(2));
    double thrust_azimuth_right =
        K_right * _realtimevessel.rotation(2) * _realtimevessel.rotation(2);
    upper_delta_u_right = std::min(
        K_right * (_realtimevessel.rotation(2) + max_delta_rotation_azimuth) *
                (_realtimevessel.rotation(2) + max_delta_rotation_azimuth) -
            thrust_azimuth_right,
        max_thrust_azimuth_right - thrust_azimuth_right);
    if (_realtimevessel.rotation(2) <= max_delta_rotation_azimuth)
      lower_delta_u_right = -thrust_azimuth_right;
    else
      lower_delta_u_right =
          K_right * (_realtimevessel.rotation(2) - max_delta_rotation_azimuth) *
              (_realtimevessel.rotation(2) - max_delta_rotation_azimuth) -
          thrust_azimuth_right;
  }

  // calculate Balpha as function of alpha
  Eigen::Matrix3d calculateBalpha(const Eigen::Vector3d &t_alpha) {
    Eigen::Matrix3d _B_alpha = Eigen::Matrix3d::Zero();
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
  double calculateRhoTerm(const Eigen::Vector3d &t_alpha, double epsilon = 0.01,
                          double rho = 10) {
    auto _B_alpha = calculateBalpha(t_alpha);
    Eigen::Matrix3d BBT = _B_alpha * _B_alpha.transpose();
    return rho / (epsilon + BBT.determinant());
  }
  // calculate Jacobian using central difference
  void calculateJocobianRhoTerm(const Eigen::Vector3d &t_alpha) {
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
  Eigen::Vector3d calculateBalphau(const Eigen::Vector3d &t_alpha,
                                   const Eigen::Vector3d &t_u) {
    return calculateBalpha(t_alpha) * t_u;
  }
  // calculate derivative of Balpha times u
  void calculateJocobianBalphaU(const Eigen::Vector3d &t_alpha,
                                const Eigen::Vector3d &t_u) {
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
  void calculateDeltauQ(const Eigen::Vector3d &t_u) {
    Eigen::Vector3d d_utemp = Eigen::Vector3d::Zero();
    d_utemp = t_u.cwiseSqrt();
    g_deltau = 1.5 * d_utemp;
    Eigen::Vector3d Q_temp = Eigen::Vector3d::Zero();
    Q_temp = 0.75 * d_utemp.cwiseInverse();
    Q_deltau = Q_temp.asDiagonal();
  }
  // calculate the BalphaU and b
  void calculateb(const realtimevessel_second &_realtimevessel) {
    b = _realtimevessel.tau - _realtimevessel.BalphaU;
  }
  // update parameters in thruster allocation for each time step
  void updateTAparameters(const realtimevessel_second &_realtimevessel) {
    B_alpha = calculateBalpha(_realtimevessel.alpha);
    calculateJocobianRhoTerm(_realtimevessel.alpha);
    calculateJocobianBalphaU(_realtimevessel.alpha, _realtimevessel.u);
    calculateDeltauQ(_realtimevessel.u);
    calculateb(_realtimevessel);
    calculateconstrains_bowthruster(_realtimevessel);
    calculateconstrains_azimuth(_realtimevessel);
  }

  void updateMosekparameters(int index = 0) {
    // update A values
    aval[0] = B_alpha(0, 0);
    aval[1] = B_alpha(1, 0);
    aval[2] = B_alpha(2, 0);
    aval[3] = B_alpha(0, 1);
    aval[4] = B_alpha(1, 1);
    aval[5] = B_alpha(2, 1);
    aval[6] = B_alpha(0, 2);
    aval[7] = B_alpha(1, 2);
    aval[8] = B_alpha(2, 2);
    aval[9] = d_Balpha_u(0, 0);
    aval[10] = d_Balpha_u(1, 0);
    aval[11] = d_Balpha_u(2, 0);
    aval[12] = d_Balpha_u(0, 1);
    aval[13] = d_Balpha_u(1, 1);
    aval[14] = d_Balpha_u(2, 1);
    aval[15] = d_Balpha_u(0, 2);
    aval[16] = d_Balpha_u(1, 2);
    aval[17] = d_Balpha_u(2, 2);

    // update linear constraints
    for (int i = 0; i != num_constraints; ++i) {
      blc[i] = b(i);
      buc[i] = b(i);
    }
    // update variable constraints

    blx[0] = lower_delta_u_bow[index];
    blx[1] = lower_delta_u_left;
    blx[2] = lower_delta_u_right;
    blx[3] = lower_delta_alpha_bow[index];
    blx[4] = lower_delta_alpha_left;
    blx[5] = lower_delta_alpha_right;

    bux[0] = upper_delta_u_bow[index];
    bux[1] = upper_delta_u_left;
    bux[2] = upper_delta_u_right;
    bux[3] = upper_delta_alpha_bow[index];
    bux[4] = upper_delta_alpha_left;
    bux[5] = upper_delta_alpha_right;

    // update objective g
    g[0] = g_deltau(0);
    g[1] = g_deltau(1);
    g[2] = g_deltau(2);
    g[3] = d_rho(0);
    g[4] = d_rho(1);
    g[5] = d_rho(2);

    // The lower triangular part of the quadratic objective Q matrix in the
    // objective is specified.
    for (int i = 0; i != n; ++i) qval[i] = Q_deltau(i, i);
  }

  // update parameters in QP for each time step
  void onestepmosek(FILE *t_file, int index = 0) {
    MSKint32t i, j;
    double t_results[numvar];

    if (r == MSK_RES_OK) {
      for (j = 0; j < numvar; ++j) {
        /* Set the linear term g_j in the objective.*/
        r = MSK_putcj(task, j, g[j]);

        /* Set the bounds on variable j.
         blx[j] <= x_j <= bux[j] */

        r = MSK_putvarbound(task, j, /* Index of variable.*/
                            bkx[j],  /* Bound key.*/
                            blx[j],  /* Numerical value of lower bound.*/
                            bux[j]); /* Numerical value of upper bound.*/

        /* Input column j of A */
        r = MSK_putacol(
            task, j,             /* Variable (column) index.*/
            aptre[j] - aptrb[j], /* Number of non-zeros in column j.*/
            asub + aptrb[j],     /* Pointer to row indexes of column j.*/
            aval + aptrb[j]);    /* Pointer to Values of column j.*/
      }

      /* Set the bounds on constraints.
         for i=1, ...,NUMCON : blc[i] <= constraint i <= buc[i] */
      for (i = 0; i < num_constraints; ++i)
        r = MSK_putconbound(task, i, /* Index of constraint.*/
                            bkc[i],  /* Bound key.*/
                            blc[i],  /* Numerical value of lower bound.*/
                            buc[i]); /* Numerical value of upper bound.*/

      /* Input the Q for the objective. */
      r = MSK_putqobj(task, 9, qsubi, qsubj, qval);

      if (r == MSK_RES_OK) {
        MSKrescodee trmcode;

        /* Run optimizer */
        r = MSK_optimizetrm(task, &trmcode);

        /* Print a summary containing information
           about the solution for debugging purposes*/
        // MSK_solutionsummary(task, MSK_STREAM_MSG);

        if (r == MSK_RES_OK) {
          MSKsolstae solsta;
          int j;

          MSK_getsolsta(task, MSK_SOL_ITR, &solsta);

          switch (solsta) {
            case MSK_SOL_STA_OPTIMAL:
            case MSK_SOL_STA_NEAR_OPTIMAL:
              MSK_getxx(task, MSK_SOL_ITR, /* Request the interior solution. */
                        t_results);

              for (j = 0; j < numvar; ++j) {
                results(j, index) = t_results[j];
              }

              break;

            case MSK_SOL_STA_DUAL_INFEAS_CER:
            case MSK_SOL_STA_PRIM_INFEAS_CER:
            case MSK_SOL_STA_NEAR_DUAL_INFEAS_CER:
            case MSK_SOL_STA_NEAR_PRIM_INFEAS_CER: {
              fprintf(
                  t_file,
                  "Second: Primal or dual infeasibility certificate found.\n");
              fclose(t_file);
              break;
            }
            case MSK_SOL_STA_UNKNOWN: {
              t_file = fopen(logsavepath.c_str(), "a+");
              fprintf(t_file,
                      "Second: The status of the solution could not be "
                      "determined.\n");
              fclose(t_file);
              break;
            }
            default: {
              t_file = fopen(logsavepath.c_str(), "a+");
              fprintf(t_file, "Second: Other solution status.");
              fclose(t_file);
              break;
            }
          }
        } else {
          t_file = fopen(logsavepath.c_str(), "a+");
          fprintf(t_file, "Second: Error while optimizing.\n");
          fclose(t_file);
        }
      }
      if (r != MSK_RES_OK) {
        /* In case of an error print error code and description. */
        char symname[MSK_MAX_STR_LEN];
        char desc[MSK_MAX_STR_LEN];
        t_file = fopen(logsavepath.c_str(), "a+");
        fprintf(t_file, "Second: An error occurred while optimizing.\n");
        MSK_getcodedesc(r, symname, desc);
        fprintf(t_file, "Error %s - '%s'\n", symname, desc);
        fclose(t_file);
      }
    }
  }
  // compare two scenario for bow thruster
  int comparebowthruster() {
    Eigen::Vector3d S1 = results.block(2 * m, 0, n, 1);
    Eigen::Vector3d S2 = results.block(2 * m, 1, n, 1);

    double position_residual1 = S1.transpose() * (Q * S1);
    double position_residual2 = S2.transpose() * (Q * S2);

    if (position_residual1 < position_residual2)
      return 0;
    else
      return 1;
  }

  // calculate vessel parameters at the next time step
  void updateNextstep(realtimevessel_second &_realtimevessel, int index) {
    // calculate delta variable using Mosek results
    delta_u = results.block(0, index, m, 1);
    delta_alpha = results.block(m, index, m, 1);

    // update alpha and u
    updateAlphaandU(_realtimevessel);
    // convert the double alpha(rad) to int alpha(deg)
    convertdoublealpha2int(_realtimevessel);
    // update rotation speed
    calculaterotation(_realtimevessel);
    // update BalphaU
    _realtimevessel.BalphaU =
        calculateBalphau(_realtimevessel.alpha, _realtimevessel.u);
  }

  // update alpha and u using computed delta_alpha and delta_u
  void updateAlphaandU(realtimevessel_second &_realtimevessel) {
    _realtimevessel.u += delta_u;
    _realtimevessel.alpha += delta_alpha;
  }
  // the precision of alpha of each azimuth thruster is 1 deg
  void convertdoublealpha2int(realtimevessel_second &_realtimevessel) {
    for (int i = 0; i != m; ++i) {
      _realtimevessel.alpha_deg(i) =
          (int)(_realtimevessel.alpha(i) * 180 / M_PI);  // round to int (deg)
      _realtimevessel.alpha(i) = _realtimevessel.alpha_deg(i) * M_PI / 180;
    }
  }
  // calcuate rotation speed of each thruster based on thrust
  void calculaterotation(realtimevessel_second &_realtimevessel) {
    int t_rotation = 0;
    // bow thruster
    if (_realtimevessel.alpha(0) < 0) {
      t_rotation = (int)(sqrt(abs(_realtimevessel.u(0)) / Kbar_negative));
      if (t_rotation == 0) {
        _realtimevessel.rotation(0) = -1;  // prevent zero
        _realtimevessel.u(0) = Kbar_negative;
      } else
        _realtimevessel.rotation(0) = -t_rotation;

    } else {
      t_rotation = (int)(sqrt(abs(_realtimevessel.u(0)) / Kbar_positive));
      if (t_rotation == 0) {
        _realtimevessel.rotation(0) = 1;  // prevent zero
        _realtimevessel.u(0) = Kbar_positive;
      } else
        _realtimevessel.rotation(0) = t_rotation;
    }
    // azimuth thruster Left
    t_rotation = (int)(sqrt(abs(_realtimevessel.u(1)) / K_left));
    if (t_rotation == 0) {
      _realtimevessel.rotation(1) = 1;
      _realtimevessel.u(1) = K_left;
    } else
      _realtimevessel.rotation(1) = t_rotation;
    // azimuth thruster Right
    t_rotation = (int)(sqrt(abs(_realtimevessel.u(2)) / K_right));
    if (t_rotation == 0) {
      _realtimevessel.rotation(2) = 1;
      _realtimevessel.u(2) = K_right;
    } else
      _realtimevessel.rotation(2) = t_rotation;
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
};

class thrusterallocation_third {
  friend void output2csv(const thrusterallocation_third &, const savefile &,
                         const std::string &);
  friend void outputmatrices(const thrusterallocation_third &);

 public:
  explicit thrusterallocation_third(const vessel_third &_vessel_third,
                                    realtimevessel_third &_realtimevessel)
      : index_twice_bow(0),
        upper_delta_alpha_left(0),
        lower_delta_alpha_left(0),
        upper_delta_u_left(0),
        lower_delta_u_left(0),
        upper_delta_alpha_right(0),
        lower_delta_alpha_right(0),
        upper_delta_u_right(0),
        lower_delta_u_right(0),
        derivative_dx(1e-5),
        aptrb{0, 3, 6, 9, 12, 15, 18, 19, 20},
        aptre{3, 6, 9, 12, 15, 18, 19, 20, 21},
        asub{0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2},
        aval{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
        bkc{MSK_BK_FX, MSK_BK_FX, MSK_BK_FX},
        blc{0, 0, 0},
        buc{0, 0, 0},
        bkx{MSK_BK_RA, MSK_BK_RA, MSK_BK_RA, MSK_BK_RA, MSK_BK_RA,
            MSK_BK_RA, MSK_BK_FR, MSK_BK_FR, MSK_BK_FR},
        blx{0, 0, 0, 0, 0, 0, -MSK_INFINITY, -MSK_INFINITY, -MSK_INFINITY},
        bux{0, 0, 0, 0, 0, 0, +MSK_INFINITY, +MSK_INFINITY, +MSK_INFINITY},
        g{0, 0, 0, 0, 0, 0, 0, 0, 0},
        qsubi{0, 1, 2, 3, 4, 5, 6, 7, 8},
        qsubj{0, 1, 2, 3, 4, 5, 6, 7, 8},
        qval{0, 0, 0, 0, 0, 0, 0, 0, 0} {
    initializethrusterallocation(_vessel_third, _realtimevessel);
  }
  thrusterallocation_third() = delete;
  ~thrusterallocation_third() {
    MSK_deletetask(&task);
    MSK_deleteenv(&env);
  }

  void onestepthrusterallocation(const vessel_third &_vessel,
                                 realtimevessel_third &_realtimevessel) {
    updateTAparameters(_vessel, _realtimevessel);

    int index = 0;
    if (index_twice_bow) {
      updateMosekparameters(_vessel, 0);
      onestepmosek(_vessel, 0);
      updateMosekparameters(_vessel, 1);
      onestepmosek(_vessel, 1);
      index = comparebowthruster(_vessel);

    } else {
      updateMosekparameters(_vessel);
      onestepmosek(_vessel);
    }
    updateNextstep(_vessel, _realtimevessel, index);
  }

  void test_multiplethrusterallocation(savefile &_savefile,
                                       const vessel_third &_vessel,
                                       realtimevessel_third &_realtimevessel) {
    // data saved for validation and viewer
    const int totalstep = 200;
    _savefile.save_u = Eigen::MatrixXd::Zero(_vessel.m, totalstep);
    _savefile.save_alpha = Eigen::MatrixXd::Zero(_vessel.m, totalstep);
    _savefile.save_alpha_deg = Eigen::MatrixXi::Zero(_vessel.m, totalstep);
    _savefile.save_Balphau = Eigen::MatrixXd::Zero(_vessel.n, totalstep);
    _savefile.save_tau = Eigen::MatrixXd::Zero(_vessel.n, totalstep);
    _savefile.save_rotation = Eigen::MatrixXi::Zero(_vessel.n, totalstep);

    // desired forces
    double angle = 0;
    for (int i = 0; i != 30; ++i) {
      angle = (i + 1) * M_PI / 15;
      _savefile.save_tau(2, i + 70) =
          0.0 * sin(angle) + 0.1 * std::rand() / RAND_MAX;
    }
    _savefile.save_tau.block(1, 0, 1, 100) =
        Eigen::MatrixXd::Constant(1, 100, 0.3) +
        0.0 * Eigen::MatrixXd::Random(1, 100);
    _savefile.save_tau.block(1, 100, 1, 100) =
        Eigen::MatrixXd::Constant(1, 100, 0.5) +
        0.0 * Eigen::MatrixXd::Random(1, 100);
    _savefile.save_tau.row(0) = 0.1 * Eigen::MatrixXd::Random(1, totalstep);
    for (int i = 0; i != totalstep; ++i) {
      // update tau
      _realtimevessel.tau = _savefile.save_tau.col(i);
      // thruster allocation
      onestepthrusterallocation(_vessel, _realtimevessel);
      // save variables
      _savefile.save_u.col(i) = _realtimevessel.u;
      _savefile.save_alpha.col(i) = _realtimevessel.alpha;
      _savefile.save_alpha_deg.col(i) = _realtimevessel.alpha_deg;
      _savefile.save_Balphau.col(i) = _realtimevessel.BalphaU;
      _savefile.save_rotation.col(i) = _realtimevessel.rotation;
    }
  }

 private:
  // real time constraints of bow thruster
  int index_twice_bow;
  Eigen::Vector2d upper_delta_alpha_bow;
  Eigen::Vector2d lower_delta_alpha_bow;
  Eigen::Vector2d upper_delta_u_bow;
  Eigen::Vector2d lower_delta_u_bow;
  // real time constraints on azimuth thruster
  double upper_delta_alpha_left;
  double lower_delta_alpha_left;
  double upper_delta_u_left;
  double lower_delta_u_left;
  double upper_delta_alpha_right;
  double lower_delta_alpha_right;
  double upper_delta_u_right;
  double lower_delta_u_right;

  // position of each thruster
  Eigen::Vector3d lx;
  Eigen::Vector3d ly;

  // quadratic objective
  Eigen::Matrix3d Q;
  Eigen::Matrix3d Omega;
  // real time Quadratic Objective in QP
  Eigen::Matrix3d Q_deltau;
  Eigen::Vector3d g_deltau;
  Eigen::Vector3d d_rho;  // derivative of rho term

  // real time constraint matrix in QP
  Eigen::Matrix3d B_alpha;
  Eigen::Matrix3d d_Balpha_u;  // Jocobian matrix of Balpha times u
  Eigen::Vector3d b;

  // real time physical variable in thruster allocation
  Eigen::Vector3d delta_alpha;  // rad
  Eigen::Vector3d delta_u;
  // linearized parameters
  double derivative_dx;  // step size of the derivative

  // parameters for Mosek API
  const MSKint32t aptrb[9], aptre[9], asub[21];
  double aval[21];
  /* Bounds on constraints. */
  const MSKboundkeye bkc[3];
  double blc[3];
  double buc[3];
  /* Bounds on variables. */
  const MSKboundkeye bkx[9];
  double blx[9];
  double bux[9];

  // objective g
  double g[9];

  // The lower triangular part of the quadratic objective Q matrix in the
  // objective is specified.
  const MSKint32t qsubi[9];
  const MSKint32t qsubj[9];
  double qval[9];
  // array to store the optimization results
  Eigen::Matrix<double, 9, 2> results;

  MSKenv_t env = NULL;
  MSKtask_t task = NULL;
  MSKrescodee r;

  void initializethrusterallocation(const vessel_third &_vessel,
                                    realtimevessel_third &_realtimevessel) {
    upper_delta_alpha_bow.setZero();
    lower_delta_alpha_bow.setZero();
    upper_delta_u_bow.setZero();
    lower_delta_u_bow.setZero();
    lx << _vessel.bow_x, _vessel.left_x, _vessel.right_x;
    ly << _vessel.bow_y, _vessel.left_y, _vessel.right_y;
    Q.setZero();
    Omega.setZero();
    Q_deltau.setZero();
    g_deltau.setZero();
    d_rho.setZero();
    B_alpha.setZero();
    d_Balpha_u.setZero();
    b.setZero();
    delta_alpha.setZero();
    delta_u.setZero();
    results.setZero();
    initializeQuadraticObjective();

    calculaterotation(_vessel, _realtimevessel);
    // update BalphaU
    _realtimevessel.BalphaU =
        calculateBalphau(_vessel, _realtimevessel.alpha, _realtimevessel.u);
    initializeMosekAPI(_vessel);
  }

  void initializeQuadraticObjective() {
    Q(0, 0) = 100;
    Q(1, 1) = 100;
    Q(2, 2) = 100;
    Omega(0, 0) = 0.1;
    Omega(1, 1) = 1;
    Omega(2, 2) = 1;
    qval[3] = Omega(0, 0);
    qval[4] = Omega(1, 1);
    qval[5] = Omega(2, 2);
    qval[6] = Q(0, 0);
    qval[7] = Q(1, 1);
    qval[8] = Q(2, 2);
  }

  void initializeMosekAPI(const vessel_third &_vessel) {
    /* Create the mosek environment. */
    r = MSK_makeenv(&env, NULL);
    /* Create the optimization task. */
    r = MSK_maketask(env, _vessel.num_constraints, _vessel.numvar, &task);
    // set up the threads used by mosek
    r = MSK_putintparam(task, MSK_IPAR_NUM_THREADS, QP_THREADS_USED);
    // r = MSK_linkfunctotaskstream(task, MSK_STREAM_LOG, NULL, printstr);
    r = MSK_linkfunctotaskstream(task, MSK_STREAM_LOG, NULL, NULL);
    // append num_constrainsts empty contraints
    r = MSK_appendcons(task, _vessel.num_constraints);
    // append numvar emtpy variables
    r = MSK_appendvars(task, _vessel.numvar);
  }
  // calculate the contraints of bow thruster
  void calculateconstrains_bowthruster(
      const vessel_third &_vessel,
      const realtimevessel_third &_realtimevessel) {
    if (0 < _realtimevessel.rotation(0) &&
        _realtimevessel.rotation(0) <= _vessel.max_delta_rotation_bow) {
      // specify the first case
      upper_delta_alpha_bow(0) = 0;
      lower_delta_alpha_bow(0) = 0;
      lower_delta_u_bow(0) = -_realtimevessel.u(0);
      upper_delta_u_bow(0) =
          _vessel.Kbar_positive *
              (_realtimevessel.rotation(0) + _vessel.max_delta_rotation_bow) *
              (_realtimevessel.rotation(0) + _vessel.max_delta_rotation_bow) -
          _realtimevessel.u(0);
      upper_delta_alpha_bow(1) = -M_PI;
      lower_delta_alpha_bow(1) = -M_PI;
      lower_delta_u_bow(1) = -_realtimevessel.u(0);
      upper_delta_u_bow(1) =
          _vessel.Kbar_negative *
              (_realtimevessel.rotation(0) - _vessel.max_delta_rotation_bow) *
              (_realtimevessel.rotation(0) - _vessel.max_delta_rotation_bow) -
          _realtimevessel.u(0);

      // specify whether two optimization problems are solved or not
      index_twice_bow = 1;
    } else if (-_vessel.max_delta_rotation_bow <= _realtimevessel.rotation(0) &&
               _realtimevessel.rotation(0) < 0) {
      // specify the first case
      upper_delta_alpha_bow(0) = 0;
      lower_delta_alpha_bow(0) = 0;
      lower_delta_u_bow(0) = -_realtimevessel.u(0);
      upper_delta_u_bow(0) =
          _vessel.Kbar_negative *
              (_realtimevessel.rotation(0) - _vessel.max_delta_rotation_bow) *
              (_realtimevessel.rotation(0) - _vessel.max_delta_rotation_bow) -
          _realtimevessel.u(0);
      // specify the second case
      upper_delta_alpha_bow(1) = M_PI;
      lower_delta_alpha_bow(1) = M_PI;
      lower_delta_u_bow(1) = -_realtimevessel.u(0);
      upper_delta_u_bow(1) =
          _vessel.Kbar_positive *
              (_realtimevessel.rotation(0) + _vessel.max_delta_rotation_bow) *
              (_realtimevessel.rotation(0) + _vessel.max_delta_rotation_bow) -
          _realtimevessel.u(0);
      // specify whether two optimization problems are solved or not
      index_twice_bow = 1;
    } else if (_realtimevessel.rotation(0) > _vessel.max_delta_rotation_bow) {
      lower_delta_alpha_bow(0) = 0;
      upper_delta_alpha_bow(0) = 0;
      upper_delta_u_bow(0) =
          std::min(_vessel.max_thrust_bow_positive - _realtimevessel.u(0),
                   _vessel.Kbar_positive * (_realtimevessel.rotation(0) +
                                            _vessel.max_delta_rotation_bow) *
                           (_realtimevessel.rotation(0) +
                            _vessel.max_delta_rotation_bow) -
                       _realtimevessel.u(0));
      lower_delta_u_bow(0) =
          _vessel.Kbar_positive *
              (_realtimevessel.rotation(0) - _vessel.max_delta_rotation_bow) *
              (_realtimevessel.rotation(0) - _vessel.max_delta_rotation_bow) -
          _realtimevessel.u(0);
      // specify whether two optimization problems are solved or not
      index_twice_bow = 0;
    } else {
      lower_delta_alpha_bow(0) = 0;
      upper_delta_alpha_bow(0) = 0;
      upper_delta_u_bow(0) =
          std::min(_vessel.Kbar_negative * (_realtimevessel.rotation(0) -
                                            _vessel.max_delta_rotation_bow) *
                           (_realtimevessel.rotation(0) -
                            _vessel.max_delta_rotation_bow) -
                       _realtimevessel.u(0),
                   _vessel.max_thrust_bow_negative - _realtimevessel.u(0));
      lower_delta_u_bow(0) =
          _vessel.Kbar_negative *
              (_realtimevessel.rotation(0) + _vessel.max_delta_rotation_bow) *
              (_realtimevessel.rotation(0) + _vessel.max_delta_rotation_bow) -
          _realtimevessel.u(0);
      // specify whether two optimization problems are solved or not
      index_twice_bow = 0;
    }
  }
  // calculate the consraints of azimuth thruster
  void calculateconstrains_azimuth(
      const vessel_third &_vessel,
      const realtimevessel_third &_realtimevessel) {
    // specify constriants on left azimuth
    upper_delta_alpha_left =
        std::min(_vessel.max_delta_alpha_azimuth,
                 _vessel.max_alpha_azimuth_left - _realtimevessel.alpha(1));
    lower_delta_alpha_left =
        std::max(-_vessel.max_delta_alpha_azimuth,
                 _vessel.min_alpha_azimuth_left - _realtimevessel.alpha(1));

    double thrust_azimuth_left = _vessel.K_left * _realtimevessel.rotation(1) *
                                 _realtimevessel.rotation(1);
    upper_delta_u_left =
        std::min(_vessel.K_left * (_realtimevessel.rotation(1) +
                                   _vessel.max_delta_rotation_azimuth) *
                         (_realtimevessel.rotation(1) +
                          _vessel.max_delta_rotation_azimuth) -
                     thrust_azimuth_left,
                 _vessel.max_thrust_azimuth_left - thrust_azimuth_left);
    if (_realtimevessel.rotation(1) <= _vessel.max_delta_rotation_azimuth)
      lower_delta_u_left = -thrust_azimuth_left;
    else
      lower_delta_u_left = _vessel.K_left *
                               (_realtimevessel.rotation(1) -
                                _vessel.max_delta_rotation_azimuth) *
                               (_realtimevessel.rotation(1) -
                                _vessel.max_delta_rotation_azimuth) -
                           thrust_azimuth_left;

    // specify constraints on right azimuth
    upper_delta_alpha_right =
        std::min(_vessel.max_delta_alpha_azimuth,
                 _vessel.max_alpha_azimuth_right - _realtimevessel.alpha(2));
    lower_delta_alpha_right =
        std::max(-_vessel.max_delta_alpha_azimuth,
                 _vessel.min_alpha_azimuth_right - _realtimevessel.alpha(2));
    double thrust_azimuth_right = _vessel.K_right *
                                  _realtimevessel.rotation(2) *
                                  _realtimevessel.rotation(2);
    upper_delta_u_right =
        std::min(_vessel.K_right * (_realtimevessel.rotation(2) +
                                    _vessel.max_delta_rotation_azimuth) *
                         (_realtimevessel.rotation(2) +
                          _vessel.max_delta_rotation_azimuth) -
                     thrust_azimuth_right,
                 _vessel.max_thrust_azimuth_right - thrust_azimuth_right);
    if (_realtimevessel.rotation(2) <= _vessel.max_delta_rotation_azimuth)
      lower_delta_u_right = -thrust_azimuth_right;
    else
      lower_delta_u_right = _vessel.K_right *
                                (_realtimevessel.rotation(2) -
                                 _vessel.max_delta_rotation_azimuth) *
                                (_realtimevessel.rotation(2) -
                                 _vessel.max_delta_rotation_azimuth) -
                            thrust_azimuth_right;
  }

  // calculate Balpha as function of alpha
  Eigen::Matrix3d calculateBalpha(const vessel_third &_vessel,
                                  const Eigen::Vector3d &t_alpha) {
    Eigen::Matrix3d _B_alpha = Eigen::Matrix3d::Zero();
    double angle = 0;
    double t_cos = 0;
    double t_sin = 0;
    for (int i = 0; i != _vessel.m; ++i) {
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
  double calculateRhoTerm(const vessel_third &_vessel,
                          const Eigen::Vector3d &t_alpha, double epsilon = 0.01,
                          double rho = 10) {
    auto _B_alpha = calculateBalpha(_vessel, t_alpha);
    Eigen::Matrix3d BBT = _B_alpha * _B_alpha.transpose();
    return rho / (epsilon + BBT.determinant());
  }
  // calculate Jacobian using central difference
  void calculateJocobianRhoTerm(const vessel_third &_vessel,
                                const Eigen::Vector3d &t_alpha) {
    for (int i = 0; i != _vessel.m; ++i) {
      auto alpha_plus = t_alpha;
      auto alpha_minus = t_alpha;
      alpha_plus(i) += derivative_dx;
      alpha_minus(i) -= derivative_dx;
      d_rho(i) = centraldifference(calculateRhoTerm(_vessel, alpha_plus),
                                   calculateRhoTerm(_vessel, alpha_minus),
                                   derivative_dx);
    }
  }
  // calculate the Balpha u term
  Eigen::Vector3d calculateBalphau(const vessel_third &_vessel,
                                   const Eigen::Vector3d &t_alpha,
                                   const Eigen::Vector3d &t_u) {
    return calculateBalpha(_vessel, t_alpha) * t_u;
  }
  // calculate derivative of Balpha times u
  void calculateJocobianBalphaU(const vessel_third &_vessel,
                                const Eigen::Vector3d &t_alpha,
                                const Eigen::Vector3d &t_u) {
    for (int i = 0; i != _vessel.m; ++i) {
      auto alpha_plus = t_alpha;
      auto alpha_minus = t_alpha;
      alpha_plus(i) += derivative_dx;
      alpha_minus(i) -= derivative_dx;
      d_Balpha_u.col(i) = (calculateBalphau(_vessel, alpha_plus, t_u) -
                           calculateBalphau(_vessel, alpha_minus, t_u)) /
                          (2 * derivative_dx);
    }
  }
  // calculate g_deltau and Q_deltau
  void calculateDeltauQ(const Eigen::Vector3d &t_u) {
    Eigen::Vector3d d_utemp = Eigen::Vector3d::Zero();
    d_utemp = t_u.cwiseSqrt();
    g_deltau = 1.5 * d_utemp;
    Eigen::Vector3d Q_temp = Eigen::Vector3d::Zero();
    Q_temp = 0.75 * d_utemp.cwiseInverse();
    Q_deltau = Q_temp.asDiagonal();
  }
  // calculate the BalphaU and b
  void calculateb(const realtimevessel_third &_realtimevessel) {
    b = _realtimevessel.tau - _realtimevessel.BalphaU;
  }
  // update parameters in thruster allocation for each time step
  void updateTAparameters(const vessel_third &_vessel,
                          const realtimevessel_third &_realtimevessel) {
    B_alpha = calculateBalpha(_vessel, _realtimevessel.alpha);
    calculateJocobianRhoTerm(_vessel, _realtimevessel.alpha);
    calculateJocobianBalphaU(_vessel, _realtimevessel.alpha, _realtimevessel.u);
    calculateDeltauQ(_realtimevessel.u);
    calculateb(_realtimevessel);
    calculateconstrains_bowthruster(_vessel, _realtimevessel);
    calculateconstrains_azimuth(_vessel, _realtimevessel);
  }
  // update the Quadratic objective and constraints in Mosek API
  void updateMosekparameters(const vessel_third &_vessel, int index = 0) {
    // update A values
    aval[0] = B_alpha(0, 0);
    aval[1] = B_alpha(1, 0);
    aval[2] = B_alpha(2, 0);
    aval[3] = B_alpha(0, 1);
    aval[4] = B_alpha(1, 1);
    aval[5] = B_alpha(2, 1);
    aval[6] = B_alpha(0, 2);
    aval[7] = B_alpha(1, 2);
    aval[8] = B_alpha(2, 2);
    aval[9] = d_Balpha_u(0, 0);
    aval[10] = d_Balpha_u(1, 0);
    aval[11] = d_Balpha_u(2, 0);
    aval[12] = d_Balpha_u(0, 1);
    aval[13] = d_Balpha_u(1, 1);
    aval[14] = d_Balpha_u(2, 1);
    aval[15] = d_Balpha_u(0, 2);
    aval[16] = d_Balpha_u(1, 2);
    aval[17] = d_Balpha_u(2, 2);

    // update linear constraints
    for (int i = 0; i != _vessel.num_constraints; ++i) {
      blc[i] = b(i);
      buc[i] = b(i);
    }
    // update variable constraints

    blx[0] = lower_delta_u_bow[index];
    blx[1] = lower_delta_u_left;
    blx[2] = lower_delta_u_right;
    blx[3] = lower_delta_alpha_bow[index];
    blx[4] = lower_delta_alpha_left;
    blx[5] = lower_delta_alpha_right;

    bux[0] = upper_delta_u_bow[index];
    bux[1] = upper_delta_u_left;
    bux[2] = upper_delta_u_right;
    bux[3] = upper_delta_alpha_bow[index];
    bux[4] = upper_delta_alpha_left;
    bux[5] = upper_delta_alpha_right;

    // update objective g
    g[0] = g_deltau(0);
    g[1] = g_deltau(1);
    g[2] = g_deltau(2);
    g[3] = d_rho(0);
    g[4] = d_rho(1);
    g[5] = d_rho(2);

    // The lower triangular part of the quadratic objective Q matrix in the
    // objective is specified.
    for (int i = 0; i != _vessel.n; ++i) qval[i] = Q_deltau(i, i);
  }

  // update parameters in QP for each time step
  void onestepmosek(const vessel_third &_vessel, int index = 0) {
    MSKint32t i, j;
    double t_results[_vessel.numvar];

    if (r == MSK_RES_OK) {
      for (j = 0; j < _vessel.numvar; ++j) {
        /* Set the linear term g_j in the objective.*/
        r = MSK_putcj(task, j, g[j]);

        /* Set the bounds on variable j.
         blx[j] <= x_j <= bux[j] */

        r = MSK_putvarbound(task, j, /* Index of variable.*/
                            bkx[j],  /* Bound key.*/
                            blx[j],  /* Numerical value of lower bound.*/
                            bux[j]); /* Numerical value of upper bound.*/

        /* Input column j of A */
        r = MSK_putacol(
            task, j,             /* Variable (column) index.*/
            aptre[j] - aptrb[j], /* Number of non-zeros in column j.*/
            asub + aptrb[j],     /* Pointer to row indexes of column j.*/
            aval + aptrb[j]);    /* Pointer to Values of column j.*/
      }

      /* Set the bounds on constraints.
         for i=1, ...,NUMCON : blc[i] <= constraint i <= buc[i] */
      for (i = 0; i < _vessel.num_constraints; ++i)
        r = MSK_putconbound(task, i, /* Index of constraint.*/
                            bkc[i],  /* Bound key.*/
                            blc[i],  /* Numerical value of lower bound.*/
                            buc[i]); /* Numerical value of upper bound.*/

      /* Input the Q for the objective. */
      r = MSK_putqobj(task, 9, qsubi, qsubj, qval);

      if (r == MSK_RES_OK) {
        MSKrescodee trmcode;

        /* Run optimizer */
        r = MSK_optimizetrm(task, &trmcode);

        /* Print a summary containing information
           about the solution for debugging purposes*/
        // MSK_solutionsummary(task, MSK_STREAM_MSG);

        if (r == MSK_RES_OK) {
          MSKsolstae solsta;
          int j;

          MSK_getsolsta(task, MSK_SOL_ITR, &solsta);

          switch (solsta) {
            case MSK_SOL_STA_OPTIMAL:
            case MSK_SOL_STA_NEAR_OPTIMAL:
              MSK_getxx(task, MSK_SOL_ITR, /* Request the interior solution. */
                        t_results);

              for (j = 0; j < _vessel.numvar; ++j) {
                results(j, index) = t_results[j];
              }

              break;

            case MSK_SOL_STA_DUAL_INFEAS_CER:
            case MSK_SOL_STA_PRIM_INFEAS_CER:
            case MSK_SOL_STA_NEAR_DUAL_INFEAS_CER:
            case MSK_SOL_STA_NEAR_PRIM_INFEAS_CER:
              printf("Primal or dual infeasibility certificate found.\n");
              break;

            case MSK_SOL_STA_UNKNOWN:
              printf("The status of the solution could not be determined.\n");
              break;

            default:
              printf("Other solution status.");
              break;
          }
        } else {
          printf("Error while optimizing.\n");
        }
      }
      if (r != MSK_RES_OK) {
        /* In case of an error print error code and description. */
        char symname[MSK_MAX_STR_LEN];
        char desc[MSK_MAX_STR_LEN];

        printf("An error occurred while optimizing.\n");
        MSK_getcodedesc(r, symname, desc);
        printf("Error %s - '%s'\n", symname, desc);
      }
    }
  }
  // compare two scenario for bow thruster
  int comparebowthruster(const vessel_third &_vessel) {
    Eigen::Vector3d S1 = results.block(2 * _vessel.m, 0, _vessel.n, 1);
    Eigen::Vector3d S2 = results.block(2 * _vessel.m, 1, _vessel.n, 1);

    double position_residual1 = S1.transpose() * (Q * S1);
    double position_residual2 = S2.transpose() * (Q * S2);

    if (position_residual1 < position_residual2)
      return 0;
    else
      return 1;
  }

  // calculate vessel parameters at the next time step
  void updateNextstep(const vessel_third &_vessel,
                      realtimevessel_third &_realtimevessel, int index) {
    // calculate delta variable using Mosek results
    delta_u = results.block(0, index, _vessel.m, 1);
    delta_alpha = results.block(_vessel.m, index, _vessel.m, 1);

    // update alpha and u
    updateAlphaandU(_realtimevessel);
    // convert the double alpha(rad) to int alpha(deg)
    convertdoublealpha2int(_vessel, _realtimevessel);
    // update rotation speed
    calculaterotation(_vessel, _realtimevessel);
    // update BalphaU
    _realtimevessel.BalphaU =
        calculateBalphau(_vessel, _realtimevessel.alpha, _realtimevessel.u);
  }

  // update alpha and u using computed delta_alpha and delta_u
  void updateAlphaandU(realtimevessel_third &_realtimevessel) {
    _realtimevessel.u += delta_u;
    _realtimevessel.alpha += delta_alpha;
  }
  // the precision of alpha of each azimuth thruster is 1 deg
  void convertdoublealpha2int(const vessel_third &_vessel,
                              realtimevessel_third &_realtimevessel) {
    for (int i = 0; i != _vessel.m; ++i) {
      _realtimevessel.alpha_deg(i) =
          (int)(_realtimevessel.alpha(i) * 180 / M_PI);  // round to int (deg)
      _realtimevessel.alpha(i) = _realtimevessel.alpha_deg(i) * M_PI / 180;
    }
  }
  // calcuate rotation speed of each thruster based on thrust
  void calculaterotation(const vessel_third &_vessel,
                         realtimevessel_third &_realtimevessel) {
    int t_rotation = 0;
    // bow thruster
    if (_realtimevessel.alpha(0) < 0) {
      t_rotation =
          (int)(sqrt(abs(_realtimevessel.u(0)) / _vessel.Kbar_negative));
      if (t_rotation == 0) {
        _realtimevessel.rotation(0) = -1;  // prevent zero
        _realtimevessel.u(0) = _vessel.Kbar_negative;
      } else
        _realtimevessel.rotation(0) = -t_rotation;

    } else {
      t_rotation =
          (int)(sqrt(abs(_realtimevessel.u(0)) / _vessel.Kbar_positive));
      if (t_rotation == 0) {
        _realtimevessel.rotation(0) = 1;  // prevent zero
        _realtimevessel.u(0) = _vessel.Kbar_positive;
      } else
        _realtimevessel.rotation(0) = t_rotation;
    }
    // azimuth thruster Left
    t_rotation = (int)(sqrt(abs(_realtimevessel.u(1)) / _vessel.K_left));
    if (t_rotation == 0) {
      _realtimevessel.rotation(1) = 1;
      _realtimevessel.u(1) = _vessel.K_left;
    } else
      _realtimevessel.rotation(1) = t_rotation;
    // azimuth thruster Right
    t_rotation = (int)(sqrt(abs(_realtimevessel.u(2)) / _vessel.K_right));
    if (t_rotation == 0) {
      _realtimevessel.rotation(2) = 1;
      _realtimevessel.u(2) = _vessel.K_right;
    } else
      _realtimevessel.rotation(2) = t_rotation;
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
};
#endif /* _thrusterallocation_h_ */
