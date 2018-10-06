#include "../include/pidcontroller_old.h"

void testonestep() {
  vessel vessel_First{.m = 4};
  pidcontrollerold mycontroller(vessel_First);
  mycontroller.basicQP_onestep();
  outputmatrices(mycontroller);
}

void testmultistep() {
  vessel vessel_First{.m = 4};
  vessel vessel_Second{.m = 4};
  pidcontrollerold mycontroller_first(vessel_First);
  pidcontrollerold mycontroller_second(vessel_Second);
  pidcontrollerold mycontroller_third(vessel_Second);
  // mycontroller.basicQP_multistep(0);
  mycontroller_first.ApproQP_multistep(1);
  mycontroller_second.ApproQP_multistep(1);
  mycontroller_third.ApproQP_multistep(1);
  output2csv(mycontroller_first, "first");
  output2csv(mycontroller_second, "second");
  output2csv(mycontroller_third, "third");
  // mycontroller.print();
}

int main() {
  testmultistep();
  // testonestep();
}
