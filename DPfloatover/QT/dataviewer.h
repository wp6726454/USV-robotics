#ifndef DATAVIEWER_H
#define DATAVIEWER_H
#include <Eigen/Core>
#include <chrono>
#include <thread>
#include <vector>

class dataviewer {
  friend void updatetest(dataviewer &t_dataviewer);

public:
  explicit dataviewer(int t_angle = 0) : test(10, 0), angle(t_angle) {
    vessel1_rev = Eigen::Vector4d::Zero();
    vessel2_rev = Eigen::Vector4d::Zero();
    vessel3_rev = Eigen::Matrix<double, 6, 1>::Zero();
    vessel1_rangle = Eigen::Vector2d::Zero();
    vessel2_rangle = Eigen::Vector2d::Zero();
    vessel3_rangle = Eigen::Vector4d::Zero();
  }

  ~dataviewer() {}

  std::vector<double> test;
  Eigen::Vector4d vessel1_rev;
  Eigen::Vector4d vessel2_rev;
  Eigen::Matrix<double, 6, 1> vessel3_rev;
  Eigen::Vector2d vessel1_rangle;
  Eigen::Vector2d vessel2_rangle;
  Eigen::Vector4d vessel3_rangle;
  volatile int angle;

private:
};

void updatetest(dataviewer &t_dataviewer);

#endif // DATAVIEWER_H
