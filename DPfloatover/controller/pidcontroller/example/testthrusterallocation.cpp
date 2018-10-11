#include "../include/thrusterallocation.h"

void testonestep() {
  vessel_first myvessel_first{
      {623, 0, 0, 0, 706, 444, 0, 444, 1298},  // mass
      {17, 0, 0, 0, 20, 0, 0, 0, 100},         // damping
      4,                                       // P_x
      1,                                       // P_y
      5,                                       // P_theta
      0.2,                                     // I_x
      0.1,                                     // I_y
      1,                                       // I_theta
      0.1,                                     // D_x
      0.1,                                     // D_y
      0.2,                                     // D_theta
      0.01,                                    // allowed_error_x
      0.01,                                    // allowed_error_y
      0.01,                                    // allowed_error_orientation
      26.0,                                    // maxpositive_x_thrust(N)
      25.0,                                    // maxnegative_x_thrust(N)
      6,                                       // maxpositive_y_thrust(N)
      4,                                       // maxnegative_y_thrust(N)
      11,                                      // maxpositive_Mz_thrust(N*m)
      7.6,                                     // maxnegative_Mz_thrust(N*m)
      3,                                       // m
      3,                                       // n
      9,                                       // numvar
      3,                                       // num_constraints
      5.6e-7,                                  // Kbar_positive
      2.2e-7,                                  // Kbar_negative
      100,                                     // max_delta_rotation_bow
      4000,                                    // max_rotation_bow
      8.96,                                    // max_thrust_bow_positive
      3.52,                                    // max_thrust_bow_negative
      2e-5,                                    // K_left
      2e-5,                                    // K_right
      20,                                      // max_delta_rotation_bow
      1000,                                    // max_rotation_azimuth
      20,                                      // max_thrust_azimuth_left
      20,                                      // max_thrust_azimuth_right
      0.1277,                                  // max_delta_alpha_azimuth
      M_PI,                                    // max_alpha_azimuth_left
      0,                                       // min_alpha_azimuth_left
      0,                                       // max_alpha_azimuth_right
      -M_PI,                                   // min_alpha_azimuth_right
      1.9,                                     // bow_x
      0,                                       // bow_y
      -1.893,                                  // left_x
      -0.216,                                  // left_y
      -1.893,                                  // right_x
      0.216                                    // right_y
  };
  realtimevessel_first myrealtimevessel;
  myrealtimevessel.tau.setZero();
  myrealtimevessel.alpha << -M_PI / 2, 0, 0;
  myrealtimevessel.u << 0.8, 10, 5;
  thrusterallocation_first mythrusterallocation(myvessel_first,
                                                myrealtimevessel);
  mythrusterallocation.onestepthrusterallocation(myrealtimevessel);
  outputmatrices(mythrusterallocation);
}

void testmultistep() {
  savefile mysavefile_first;
  savefile mysavefile_second;
  savefile mysavefile_third;
  vessel_first myvessel_first{
      {623, 0, 0, 0, 706, 444, 0, 444, 1298},  // mass
      {17, 0, 0, 0, 20, 0, 0, 0, 100},         // damping
      4,                                       // P_x
      1,                                       // P_y
      5,                                       // P_theta
      0.2,                                     // I_x
      0.1,                                     // I_y
      1,                                       // I_theta
      0.1,                                     // D_x
      0.1,                                     // D_y
      0.2,                                     // D_theta
      0.01,                                    // allowed_error_x
      0.01,                                    // allowed_error_y
      0.01,                                    // allowed_error_orientation
      26.0,                                    // maxpositive_x_thrust(N)
      25.0,                                    // maxnegative_x_thrust(N)
      6,                                       // maxpositive_y_thrust(N)
      4,                                       // maxnegative_y_thrust(N)
      11,                                      // maxpositive_Mz_thrust(N*m)
      7.6,                                     // maxnegative_Mz_thrust(N*m)
      3,                                       // m
      3,                                       // n
      9,                                       // numvar
      3,                                       // num_constraints
      5.6e-7,                                  // Kbar_positive
      2.2e-7,                                  // Kbar_negative
      100,                                     // max_delta_rotation_bow
      4000,                                    // max_rotation_bow
      8.96,                                    // max_thrust_bow_positive
      3.52,                                    // max_thrust_bow_negative
      2e-5,                                    // K_left
      2e-5,                                    // K_right
      20,                                      // max_delta_rotation_bow
      1000,                                    // max_rotation_azimuth
      20,                                      // max_thrust_azimuth_left
      20,                                      // max_thrust_azimuth_right
      0.1277,                                  // max_delta_alpha_azimuth
      M_PI,                                    // max_alpha_azimuth_left
      0,                                       // min_alpha_azimuth_left
      0,                                       // max_alpha_azimuth_right
      -M_PI,                                   // min_alpha_azimuth_right
      1.9,                                     // bow_x
      0,                                       // bow_y
      -1.893,                                  // left_x
      -0.216,                                  // left_y
      -1.893,                                  // right_x
      0.216                                    // right_y
  };
  vessel_third myvessel_third{
      {623, 0, 0, 0, 706, 444, 0, 444, 1298},  // mass
      {17, 0, 0, 0, 20, 0, 0, 0, 100},         // damping
      4,                                       // P_x
      1,                                       // P_y
      5,                                       // P_theta
      0.2,                                     // I_x
      0.1,                                     // I_y
      1,                                       // I_theta
      0.1,                                     // D_x
      0.1,                                     // D_y
      0.2,                                     // D_theta
      0.01,                                    // allowed_error_x
      0.01,                                    // allowed_error_y
      0.01,                                    // allowed_error_orientation
      26.0,                                    // maxpositive_x_thrust(N)
      25.0,                                    // maxnegative_x_thrust(N)
      6,                                       // maxpositive_y_thrust(N)
      4,                                       // maxnegative_y_thrust(N)
      11,                                      // maxpositive_Mz_thrust(N*m)
      7.6,                                     // maxnegative_Mz_thrust(N*m)
      3,                                       // m
      3,                                       // n
      9,                                       // numvar
      3,                                       // num_constraints
      5.6e-7,                                  // Kbar_positive
      2.2e-7,                                  // Kbar_negative
      100,                                     // max_delta_rotation_bow
      4000,                                    // max_rotation_bow
      8.96,                                    // max_thrust_bow_positive
      3.52,                                    // max_thrust_bow_negative
      2e-5,                                    // K_left
      2e-5,                                    // K_right
      20,                                      // max_delta_rotation_bow
      1000,                                    // max_rotation_azimuth
      20,                                      // max_thrust_azimuth_left
      20,                                      // max_thrust_azimuth_right
      0.1277,                                  // max_delta_alpha_azimuth
      M_PI,                                    // max_alpha_azimuth_left
      0,                                       // min_alpha_azimuth_left
      0,                                       // max_alpha_azimuth_right
      -M_PI,                                   // min_alpha_azimuth_right
      1.9,                                     // bow_x
      0,                                       // bow_y
      -1.893,                                  // left_x
      -0.216,                                  // left_y
      -1.893,                                  // right_x
      0.216                                    // right_y
  };
  vessel_second myvessel_second{
      {623, 0, 0, 0, 706, 444, 0, 444, 1298},  // mass
      {17, 0, 0, 0, 20, 0, 0, 0, 100},         // damping
      4,                                       // P_x
      1,                                       // P_y
      5,                                       // P_theta
      0.2,                                     // I_x
      0.1,                                     // I_y
      1,                                       // I_theta
      0.1,                                     // D_x
      0.1,                                     // D_y
      0.2,                                     // D_theta
      0.01,                                    // allowed_error_x
      0.01,                                    // allowed_error_y
      0.01,                                    // allowed_error_orientation
      26.0,                                    // maxpositive_x_thrust(N)
      25.0,                                    // maxnegative_x_thrust(N)
      6,                                       // maxpositive_y_thrust(N)
      4,                                       // maxnegative_y_thrust(N)
      11,                                      // maxpositive_Mz_thrust(N*m)
      7.6,                                     // maxnegative_Mz_thrust(N*m)
      3,                                       // m
      3,                                       // n
      9,                                       // numvar
      3,                                       // num_constraints
      5.6e-7,                                  // Kbar_positive
      2.2e-7,                                  // Kbar_negative
      100,                                     // max_delta_rotation_bow
      4000,                                    // max_rotation_bow
      8.96,                                    // max_thrust_bow_positive
      3.52,                                    // max_thrust_bow_negative
      2e-5,                                    // K_left
      2e-5,                                    // K_right
      20,                                      // max_delta_rotation_bow
      1000,                                    // max_rotation_azimuth
      20,                                      // max_thrust_azimuth_left
      20,                                      // max_thrust_azimuth_right
      0.1277,                                  // max_delta_alpha_azimuth
      M_PI,                                    // max_alpha_azimuth_left
      0,                                       // min_alpha_azimuth_left
      0,                                       // max_alpha_azimuth_right
      -M_PI,                                   // min_alpha_azimuth_right
      1.9,                                     // bow_x
      0,                                       // bow_y
      -1.893,                                  // left_x
      -0.216,                                  // left_y
      -1.893,                                  // right_x
      0.216                                    // right_y
  };

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