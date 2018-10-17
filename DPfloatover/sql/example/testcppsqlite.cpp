#include "../include/databasecpp.h"
int main() {
  try {
    // creates a database file 'dbfile.db' if it does not exists.
    databasecpp mydb;
    mydb.create_mastertable();
    for (int i = 0; i != MAXCONNECTION; ++i) {
      mydb.update_mastertable(i);
      mydb.create_client_table(i);
    }

    // realtime parameters of the first vessel (K class-I)
    realtimevessel_first _realtimevessel_first{
        (Vector6d() << 0.01, 0.1, 0.01, 1, 2, 4).finished(),  // measurement
        Vector6d::Zero(),                                     // position
        Vector6d::Zero(),                                     // velocity
        Vector6d::Zero(),                                     // state
        Eigen::Vector3d::Zero(),                              // tau
        Eigen::Vector3d::Zero(),                              // BalphaU
        (Eigen::Vector3d() << -M_PI / 2, M_PI / 180, -M_PI / 30)
            .finished(),                                    // alpha
        Eigen::Vector3i::Zero(),                            // alpha_deg
        (Eigen::Vector3d() << 0.01, 0.1, 0.01).finished(),  // u
        Eigen::Vector3i::Zero()                             // rotation
    };
    // realtime parameters of the second vessel (K class-II)
    realtimevessel_second _realtimevessel_second{
        Vector6d::Zero(),                                     // measurement
        Vector6d::Zero(),                                     // position
        Vector6d::Zero(),                                     // velocity
        (Vector6d() << 0.01, 0.1, 0.01, 1, 2, 4).finished(),  // state
        Eigen::Vector3d::Zero(),                              // tau
        Eigen::Vector3d::Zero(),                              // BalphaU
        (Eigen::Vector3d() << M_PI / 2, M_PI / 180, -M_PI / 30)
            .finished(),                                   // alpha
        Eigen::Vector3i::Zero(),                           // alpha_deg
        (Eigen::Vector3d() << 0.01, 0.0, 0.0).finished(),  // u
        Eigen::Vector3i::Zero()                            // rotation
    };
    mydb.update_client_table(1, _realtimevessel_first);
    mydb.update_client_table(0, _realtimevessel_first);
    mydb.update_client_table(1, _realtimevessel_second);
    mydb.update_client_table(0, _realtimevessel_second);

  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
  }
}
