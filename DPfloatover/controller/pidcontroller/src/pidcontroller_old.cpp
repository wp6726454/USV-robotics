#include "../include/pidcontroller_old.h"

// write Eigen matrix into csv file
void write2csvfile(const std::string &name, const Eigen::MatrixXd &matrix) {
  const static Eigen::IOFormat CSVFormat(Eigen::StreamPrecision,
                                         Eigen::DontAlignCols, ", ", "\n");
  std::ofstream file(name.c_str());
  if (file.is_open()) {
    file << matrix.format(CSVFormat);
  }
}

void output2csv(const pidcontrollerold &_pidcontroller,
                const std::string &_name) {
  write2csvfile(_name + "_tau.csv", _pidcontroller.save_tau);
  write2csvfile(_name + "_u.csv", _pidcontroller.save_u);
  write2csvfile(_name + "_alpha.csv", _pidcontroller.save_alpha);
  write2csvfile(_name + "_Balpha.csv", _pidcontroller.save_Balphau);
}

void outputmatrices(const pidcontrollerold &_pidcontroller) {
  // std::cout << "Q= " << std::endl << Q << std::endl;
  // std::cout << "Omega= " << std::endl << Omega << std::endl;
  // std::cout << "lx= " << std::endl << lx << std::endl;
  // std::cout << "ly= " << std::endl << ly << std::endl;
  // std::cout << "B_alpha= " << std::endl << B_alpha << std::endl;
  // std::cout << "alpha = " << std::endl << alpha << std::endl;
  // std::cout << "u= " << std::endl << u << std::endl;
  // std::cout << "delta_u= " << std::endl << delta_u << std::endl;
  // std::cout << "d_rho " << std::endl << d_rho << std::endl;
  // std::cout << "d_Balpha_u= " << std::endl << d_Balpha_u << std::endl;
  // std::cout << "g_deltau= " << std::endl << g_deltau << std::endl;
  // std::cout << "Q_deltau= " << std::endl << Q_deltau << std::endl;
  std::cout << "H= " << std::endl << _pidcontroller.H << std::endl;
  std::cout << "g= " << std::endl << _pidcontroller.g << std::endl;
  std::cout << "C= " << std::endl << _pidcontroller.C << std::endl;
  std::cout << "b= " << std::endl << _pidcontroller.b << std::endl;
  std::cout << "P= " << std::endl << _pidcontroller.P << std::endl;
  std::cout << "h= " << std::endl << _pidcontroller.h << std::endl;
  // std::cout << "z= " << std::endl << z << std::endl;
  // std::cout << "mu= " << std::endl << mu << std::endl;
  // std::cout << "delta_z= " << std::endl << delta_z << std::endl;
  // std::cout << "delta_mu= " << std::endl << delta_mu << std::endl;
  // std::cout << "delta_u= " << std::endl << delta_u << std::endl;
  // std::cout << "delta_alpha= " << std::endl << delta_alpha << std::endl;
}
