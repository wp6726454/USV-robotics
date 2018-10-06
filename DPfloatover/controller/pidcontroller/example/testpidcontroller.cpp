#include "../include/pidcontroller.h"

int main() {
  realtimevessel_first myrealvessel;
  vessel_first myvessel_first;
  myrealvessel.tau.setZero();
  myrealvessel.State << -M_PI / 2, 1, 2, 10, 2.3, 5;
  Eigen::Vector3d setPoints;
  setPoints << 0, 0, 0;
  pidcontroller mypidcontroller(myvessel_first);
  mypidcontroller.calculategeneralizeforce(myrealvessel, setPoints);
  std::cout << myrealvessel.tau << std::endl;
  mypidcontroller.calculategeneralizeforce(myrealvessel, setPoints);
  std::cout << myrealvessel.tau << std::endl;
}