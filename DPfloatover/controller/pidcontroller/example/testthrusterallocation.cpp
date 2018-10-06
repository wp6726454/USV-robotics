#include "../include/thrusterallocation.h"

void testonestep() {
  vessel_first myvessel_first;
  realtimevessel_first myrealtimevessel;
  myrealtimevessel.tau.setZero();
  myrealtimevessel.alpha << -M_PI / 2, 0, 0;
  myrealtimevessel.u << 0.8, 10, 5;
  thrusterallocation_first mythrusterallocation(myvessel_first,
                                                myrealtimevessel);
  mythrusterallocation.onestepthrusterallocation(myvessel_first,
                                                 myrealtimevessel);
  outputmatrices(mythrusterallocation);
}

void testmultistep() {
  savefile mysavefile_first;
  savefile mysavefile_second;
  savefile mysavefile_third;
  vessel_first myvessel_first;
  vessel_second myvessel_second;
  vessel_third myvessel_third;
  realtimevessel_first myrealtimevessel_first;
  realtimevessel_second myrealtimevessel_second;
  realtimevessel_third myrealtimevessel_third;
  // specify parameters of the first vessel
  myrealtimevessel_first.tau.setZero();
  myrealtimevessel_first.alpha << -M_PI / 2, M_PI / 30, -M_PI / 180;
  myrealtimevessel_first.u << 0.001, 0.1, 0.1;
  thrusterallocation_first mythrusterallocation_first(myvessel_first,
                                                      myrealtimevessel_first);
  mythrusterallocation_first.test_multiplethrusterallocation(
      mysavefile_first, myvessel_first, myrealtimevessel_first);
  output2csv(mythrusterallocation_first, mysavefile_first, "First");
  // specify parameters of the second vessel
  myrealtimevessel_second.tau.setZero();
  myrealtimevessel_second.alpha << -M_PI / 2, M_PI / 30, -M_PI / 180;
  myrealtimevessel_second.u << 0.001, 0.1, 0.1;
  thrusterallocation_second mythrusterallocation_second(
      myvessel_second, myrealtimevessel_second);
  mythrusterallocation_second.test_multiplethrusterallocation(
      mysavefile_second, myvessel_second, myrealtimevessel_second);
  output2csv(mythrusterallocation_second, mysavefile_second, "Second");
  // specify parameters of the third vessel
  myrealtimevessel_third.tau.setZero();
  myrealtimevessel_third.alpha << -M_PI / 2, M_PI / 30, -M_PI / 180;
  myrealtimevessel_third.u << 0.001, 0.1, 0.1;
  thrusterallocation_third mythrusterallocation_third(myvessel_third,
                                                      myrealtimevessel_third);
  mythrusterallocation_third.test_multiplethrusterallocation(
      mysavefile_third, myvessel_third, myrealtimevessel_third);
  output2csv(mythrusterallocation_third, mysavefile_third, "Third");
}
int main() { testmultistep(); }