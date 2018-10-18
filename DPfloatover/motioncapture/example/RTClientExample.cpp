#include "motioncapture.h"

int main(int argc, char** argv) {
  // realtime parameters of the first vessel (K class-I)
  realtimevessel_first _realtimevessel_first{
      Vector6d::Zero(),         // measurement
      Vector6d::Zero(),         // position
      Vector6d::Zero(),         // velocity
      Vector6d::Zero(),         // state
      Eigen::Vector3d::Zero(),  // tau
      Eigen::Vector3d::Zero(),  // BalphaU
      (Eigen::Vector3d() << -M_PI / 2, M_PI / 180, -M_PI / 30)
          .finished(),                                    // alpha
      Eigen::Vector3i::Zero(),                            // alpha_deg
      (Eigen::Vector3d() << 0.01, 0.1, 0.01).finished(),  // u
      Eigen::Vector3i::Zero()                             // rotation
  };
  // realtime parameters of the second vessel (K class-II)
  realtimevessel_second _realtimevessel_second{
      Vector6d::Zero(),         // measurement
      Vector6d::Zero(),         // position
      Vector6d::Zero(),         // velocity
      Vector6d::Zero(),         // state
      Eigen::Vector3d::Zero(),  // tau
      Eigen::Vector3d::Zero(),  // BalphaU
      (Eigen::Vector3d() << M_PI / 2, M_PI / 180, -M_PI / 30)
          .finished(),                                   // alpha
      Eigen::Vector3i::Zero(),                           // alpha_deg
      (Eigen::Vector3d() << 0.01, 0.0, 0.0).finished(),  // u
      Eigen::Vector3i::Zero()                            // rotation
  };
  // realtime parameters of the third vessel (X class)
  realtimevessel_third _realtimevessel_third{
      Vector6d::Zero(),         // measurement
      Vector6d::Zero(),         // position
      Vector6d::Zero(),         // velocity
      Vector6d::Zero(),         // state
      Eigen::Vector3d::Zero(),  // tau
      Eigen::Vector3d::Zero(),  // BalphaU
      (Eigen::Vector3d() << -M_PI / 2, M_PI / 180, -M_PI / 30)
          .finished(),                                    // alpha
      Eigen::Vector3i::Zero(),                            // alpha_deg
      (Eigen::Vector3d() << 0.001, 0.0, 0.0).finished(),  // u
      Eigen::Vector3i::Zero()                             // rotation
  };

  motioncapture _motioncapture;

  if (_motioncapture.initializemotioncapture() == 0) {
    _motioncapture.RequestPositionandVelocity(
        _realtimevessel_first, _realtimevessel_second, _realtimevessel_third);
  }
  return 1;
}  // main