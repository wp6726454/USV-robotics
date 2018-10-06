#include "../include/controller.h"

void testSchurcontroller() {
  controller mycontroller;
  mycontroller.SchurComposition();
  mycontroller.print();
}

void testfastMPC() {
  controller mycontrollers;
  mycontrollers.FastMPCcomputtion();
  mycontrollers.print();
}

void testcompare() {
  controller mycontrollers;
  mycontrollers.compareTwoMethods();
  mycontrollers.print();
}

void testfastmpc() {
  controller mycontroller;
  // mycontroller.print();
  mycontroller.basicQP_multistep();
  // mycontroller.approximateQP_multistep();
  mycontroller.output();
}

void testfastmpcone_basic() {
  controller mycontroller;
  mycontroller.basicQP_onestep();
}

void testfastmpcone_appro() {
  controller mycontroller;
  mycontroller.approximateQP_onestep();
}

void testresidual() {
  controller mycontroller;
  mycontroller.testcomputeresidual();
  mycontroller.output();
}

int main() {
  // testfastmpcone_appro();
  testfastmpc();
  // testcompare();
}
