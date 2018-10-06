/*
***********************************************************************
* eigenmvnd.hpp: Multivariate Normal distribution sampling
* function to simulate the multivariate normal distribution using C++11
* and Eigen matrices.
* This header file can be read by C++ compilers
*
*  by Hu.ZH(Mr.SJTU)
***********************************************************************
*/

#ifndef _EIGENMVND_HPP
#define _EIGENMVND_HPP

#include <Eigen/Dense>
#include <cstdlib>
#include <ctime>
#include <initializer_list>
#include <iostream>
#include <map>
#include <random>
#include <string>
#include <utility>
#include <vector>

class eigenmvnd {
  friend std::ostream &operator<<(std::ostream &os,
                                  eigenmvnd const &requested_ceigen) {
    os << "Mean vector: \n"
       << requested_ceigen.mean << "Covariance matrix: \n"
       << requested_ceigen.covariance << "Cholesky results: \n"
       << requested_ceigen.multidistribution << '\n';
    return os;
  }

 public:
  // disable the default constructor
  eigenmvnd() = delete;
  // default constructor
  explicit eigenmvnd(const Eigen::MatrixXd &requested_mean,
                     const Eigen::MatrixXd &requested_covariance,
                     int _totalsample) noexcept
      : mean(requested_mean),
        covariance(requested_covariance),
        totalsample(_totalsample),
        row_standard(requested_mean.rows()),
        multidistribution(row_standard, totalsample) {}

  ~eigenmvnd() noexcept {}

  eigenmvnd &perform_mvnd() {
    // generate a M*N matrix with multivariate normal distribution
    multidistribution =
        generate_meanm() +
        generate_cholesky_matrix_L() * generate_standard_normals();
    return *this;
  }

  auto get_mvnd_matrix() const noexcept { return multidistribution; }

 private:
  // utility functions
  Eigen::MatrixXd generate_standard_normal() const {
    // generate a column vector with standard normal distribution
    std::random_device rd{};
    std::mt19937 generator{rd()};
    std::normal_distribution<> distribution{0, 1};
    auto normal = [&](double) { return distribution(generator); };
    return Eigen::MatrixXd::NullaryExpr(row_standard, 1, normal);
  }

  Eigen::MatrixXd generate_standard_normals() const {
    // generate a matrix with standard normal distribution
    std::random_device rd{};
    std::mt19937 generator{rd()};
    std::normal_distribution<> distribution{0, 1};
    auto normal = [&](double) { return distribution(generator); };
    return Eigen::MatrixXd::NullaryExpr(row_standard, totalsample, normal);
  }

  Eigen::MatrixXd generate_cholesky_matrix_L() const {
    // perform cholesky decomposition
    Eigen::LLT<decltype(covariance)> lltofA(covariance);
    return static_cast<decltype(covariance)>(lltofA.matrixL());
  }

  Eigen::MatrixXd generate_meanm() const {
    // generate the mean matrix repeatly
    return mean.replicate(1, totalsample);
  }

  Eigen::MatrixXd mean;
  Eigen::MatrixXd covariance;
  int totalsample;
  int row_standard;
  Eigen::MatrixXd multidistribution;
};
#endif  // _EIGENMVND_HPP
