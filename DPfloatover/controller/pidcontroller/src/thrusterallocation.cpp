#include "../include/thrusterallocation.h"

void outputmatrices(const thrusterallocation_first &_thrusterallocation) {
  std::cout << "upper_delta_u_bow = " << _thrusterallocation.upper_delta_u_bow
            << std::endl;
  std::cout << "lower_delta_u_bow = " << _thrusterallocation.lower_delta_u_bow
            << std::endl;
  std::cout << "upper_delta_alpha_bow = "
            << _thrusterallocation.upper_delta_alpha_bow << std::endl;
  std::cout << "lower_delta_alpha_bow = "
            << _thrusterallocation.lower_delta_alpha_bow << std::endl;

  std::cout << "upper_delta_alpha_left = "
            << _thrusterallocation.upper_delta_alpha_left << std::endl;
  std::cout << "lower_delta_alpha_left = "
            << _thrusterallocation.lower_delta_alpha_left << std::endl;
  std::cout << "upper_delta_u_left = " << _thrusterallocation.upper_delta_u_left
            << std::endl;
  std::cout << "lower_delta_u_left = " << _thrusterallocation.lower_delta_u_left
            << std::endl;
  std::cout << "upper_delta_alpha_right = "
            << _thrusterallocation.upper_delta_alpha_right << std::endl;
  std::cout << "lower_delta_alpha_right = "
            << _thrusterallocation.lower_delta_alpha_right << std::endl;
  std::cout << "upper_delta_u_right = "
            << _thrusterallocation.upper_delta_u_right << std::endl;
  std::cout << "lower_delta_u_right = "
            << _thrusterallocation.lower_delta_u_right << std::endl;

  std::cout << "B_alpha = " << std::endl
            << _thrusterallocation.B_alpha << std::endl;
  std::cout << "d_rho = " << std::endl
            << _thrusterallocation.d_rho << std::endl;
  std::cout << "d_Balpha_u = " << std::endl
            << _thrusterallocation.d_Balpha_u << std::endl;
  std::cout << "g_deltau = " << std::endl
            << _thrusterallocation.g_deltau << std::endl;
  std::cout << "Q_deltau = " << std::endl
            << _thrusterallocation.Q_deltau << std::endl;
}

void outputmatrices(const thrusterallocation_second &_thrusterallocation) {
  std::cout << "upper_delta_u_bow = " << _thrusterallocation.upper_delta_u_bow
            << std::endl;
  std::cout << "lower_delta_u_bow = " << _thrusterallocation.lower_delta_u_bow
            << std::endl;
  std::cout << "upper_delta_alpha_bow = "
            << _thrusterallocation.upper_delta_alpha_bow << std::endl;
  std::cout << "lower_delta_alpha_bow = "
            << _thrusterallocation.lower_delta_alpha_bow << std::endl;

  std::cout << "upper_delta_alpha_left = "
            << _thrusterallocation.upper_delta_alpha_left << std::endl;
  std::cout << "lower_delta_alpha_left = "
            << _thrusterallocation.lower_delta_alpha_left << std::endl;
  std::cout << "upper_delta_u_left = " << _thrusterallocation.upper_delta_u_left
            << std::endl;
  std::cout << "lower_delta_u_left = " << _thrusterallocation.lower_delta_u_left
            << std::endl;
  std::cout << "upper_delta_alpha_right = "
            << _thrusterallocation.upper_delta_alpha_right << std::endl;
  std::cout << "lower_delta_alpha_right = "
            << _thrusterallocation.lower_delta_alpha_right << std::endl;
  std::cout << "upper_delta_u_right = "
            << _thrusterallocation.upper_delta_u_right << std::endl;
  std::cout << "lower_delta_u_right = "
            << _thrusterallocation.lower_delta_u_right << std::endl;

  std::cout << "B_alpha = " << std::endl
            << _thrusterallocation.B_alpha << std::endl;
  std::cout << "d_rho = " << std::endl
            << _thrusterallocation.d_rho << std::endl;
  std::cout << "d_Balpha_u = " << std::endl
            << _thrusterallocation.d_Balpha_u << std::endl;
  std::cout << "g_deltau = " << std::endl
            << _thrusterallocation.g_deltau << std::endl;
  std::cout << "Q_deltau = " << std::endl
            << _thrusterallocation.Q_deltau << std::endl;
}

void outputmatrices(const thrusterallocation_third &_thrusterallocation) {
  std::cout << "upper_delta_u_bow = " << _thrusterallocation.upper_delta_u_bow
            << std::endl;
  std::cout << "lower_delta_u_bow = " << _thrusterallocation.lower_delta_u_bow
            << std::endl;
  std::cout << "upper_delta_alpha_bow = "
            << _thrusterallocation.upper_delta_alpha_bow << std::endl;
  std::cout << "lower_delta_alpha_bow = "
            << _thrusterallocation.lower_delta_alpha_bow << std::endl;

  std::cout << "upper_delta_alpha_left = "
            << _thrusterallocation.upper_delta_alpha_left << std::endl;
  std::cout << "lower_delta_alpha_left = "
            << _thrusterallocation.lower_delta_alpha_left << std::endl;
  std::cout << "upper_delta_u_left = " << _thrusterallocation.upper_delta_u_left
            << std::endl;
  std::cout << "lower_delta_u_left = " << _thrusterallocation.lower_delta_u_left
            << std::endl;
  std::cout << "upper_delta_alpha_right = "
            << _thrusterallocation.upper_delta_alpha_right << std::endl;
  std::cout << "lower_delta_alpha_right = "
            << _thrusterallocation.lower_delta_alpha_right << std::endl;
  std::cout << "upper_delta_u_right = "
            << _thrusterallocation.upper_delta_u_right << std::endl;
  std::cout << "lower_delta_u_right = "
            << _thrusterallocation.lower_delta_u_right << std::endl;

  std::cout << "B_alpha = " << std::endl
            << _thrusterallocation.B_alpha << std::endl;
  std::cout << "d_rho = " << std::endl
            << _thrusterallocation.d_rho << std::endl;
  std::cout << "d_Balpha_u = " << std::endl
            << _thrusterallocation.d_Balpha_u << std::endl;
  std::cout << "g_deltau = " << std::endl
            << _thrusterallocation.g_deltau << std::endl;
  std::cout << "Q_deltau = " << std::endl
            << _thrusterallocation.Q_deltau << std::endl;
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
// write Eigen matrix into csv file
void write2csvfile(const std::string &name, const Eigen::MatrixXi &matrix) {
  const static Eigen::IOFormat CSVFormat(Eigen::StreamPrecision,
                                         Eigen::DontAlignCols, ", ", "\n");
  std::ofstream file(name.c_str());
  if (file.is_open()) {
    file << matrix.format(CSVFormat);
  }
}

void output2csv(const thrusterallocation_first &_thrusterallocation,
                const savefile &_savefile, const std::string &_name) {
  write2csvfile(_name + "_tau.csv", _savefile.save_tau);
  write2csvfile(_name + "_u.csv", _savefile.save_u);
  write2csvfile(_name + "_alpha.csv", _savefile.save_alpha);
  write2csvfile(_name + "_alpha_deg.csv", _savefile.save_alpha_deg);
  write2csvfile(_name + "_Balpha.csv", _savefile.save_Balphau);
  write2csvfile(_name + "_rotation.csv", _savefile.save_rotation);
}

void output2csv(const thrusterallocation_second &_thrusterallocation,
                const savefile &_savefile, const std::string &_name) {
  write2csvfile(_name + "_tau.csv", _savefile.save_tau);
  write2csvfile(_name + "_u.csv", _savefile.save_u);
  write2csvfile(_name + "_alpha.csv", _savefile.save_alpha);
  write2csvfile(_name + "_alpha_deg.csv", _savefile.save_alpha_deg);
  write2csvfile(_name + "_Balpha.csv", _savefile.save_Balphau);
  write2csvfile(_name + "_rotation.csv", _savefile.save_rotation);
}

void output2csv(const thrusterallocation_third &_thrusterallocation,
                const savefile &_savefile, const std::string &_name) {
  write2csvfile(_name + "_tau.csv", _savefile.save_tau);
  write2csvfile(_name + "_u.csv", _savefile.save_u);
  write2csvfile(_name + "_alpha.csv", _savefile.save_alpha);
  write2csvfile(_name + "_alpha_deg.csv", _savefile.save_alpha_deg);
  write2csvfile(_name + "_Balpha.csv", _savefile.save_Balphau);
  write2csvfile(_name + "_rotation.csv", _savefile.save_rotation);
}
