/*
***********************************************************************
* thread_Loop.hpp: thread-based concurrent socket server
* function to run the whole loop on server (including receive from clients,
* crccheck, unpack, 6D motion capture, LQE, LQR, thruster allocation, pack, send
* to clients, save2sqlite, viewer).
* This header file can be read by C++ compilers
*
*  by Hu.ZH(Mr.SJTU)
***********************************************************************
*/

#ifndef _THREADLOOP_HPP_
#define _THREADLOOP_HPP_

#include <pthread.h>
#include <sys/prctl.h>
#include <unistd.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/timer.hpp>
#include <chrono>
#include <cstdlib>
#include <thread>
#include <unordered_map>
#include "../controller/pidcontroller/include/controller.h"
#include "../joystick/include/gamepadmonitor.h"
#include "../motioncapture/include/motioncapture.h"
#include "../network/include/crccheck.h"
#include "../network/include/datapack.h"
#include "../network/include/pnserver_t.h"
#include "../sql/include/databasecpp.h"
#include "constants.h"
#include "timecounter.hpp"

class threadloop {
 public:
  threadloop()
      : connection_status(0),
        mydb(dbsavepath),
        _controller_first(_vessel_first, _realtimevessel_first),
        _controller_second(_vessel_second, _realtimevessel_second),
        _controller_third(_vessel_third, _realtimevessel_third) {}
  ~threadloop() {}

  void initializelooop() {
    // open system file descriptors
    if (FILEORNOT) {
      myfile = fopen(logsavepath.c_str(), "a+");
      // initialize pn server
      pnd_init();
    } else {
      // initialize socket server
      pnd_init();
    }
    // sqlite to create master table
    mydb.create_mastertable();
  }
  // start socket server and wait for clients
  void start_connnection_t() {
    if (_SERV_CP_Startup() == 0) {
      _opencontroller();
      pnd_test_set_mode(PNIO_MODE_OPERATE);
      createtables();
    }
  }
  // loop for send and recive data using socket
  void controller_t() {
    if (MAXCONNECTION > 0) {
      // thread for the first vessel
      std::thread _threadfirst(&threadloop::controller_first_pn, this);
      if (FILEORNOT) {  // join for terminal, detach for QT
        _tmclients[0] = _threadfirst.native_handle();
        _threadfirst.detach();
      } else
        _threadfirst.join();
    }
    if (MAXCONNECTION > 1) {
      // thread for the second vessel
      std::thread _threadsecond(&threadloop::controller_second_pn, this);
      if (FILEORNOT) {  // join for terminal, detach for QT
        _tmclients[1] = _threadsecond.native_handle();
        _threadsecond.detach();
      } else
        _threadsecond.join();
    }
    if (MAXCONNECTION > 2) {
      // thread for the third vessel
      std::thread _threadthird(&threadloop::controller_third_pn, this);
      if (FILEORNOT) {  // join for terminal, detach for QT
        _tmclients[2] = _threadthird.native_handle();
        _threadthird.detach();
      } else
        _threadthird.join();
    }
  }
  // profinet
  void send2allclients_pn_t() {
    std::thread _threadpnsend(&threadloop::send2allclients_pn, this);
    if (FILEORNOT) {  // join for terminal, detach for QT
      _threadid_pnsend = _threadpnsend.native_handle();
      _threadpnsend.detach();
    } else
      _threadpnsend.join();
  }
  // gampad
  void updategamepad_t() {
    std::thread _threadgamepad(&threadloop::updategamepad, this);
    if (FILEORNOT) {  // join for terminal, detach for QT
      _threadid_gamepad = _threadgamepad.native_handle();
      _threadgamepad.detach();
    } else
      _threadgamepad.join();
  }
  // motion capture
  void updatemotioncapture_t() {
    std::thread _threadmotioncapure(&threadloop::updatemotion, this);
    if (FILEORNOT) {  // join for terminal, detach for QT
      _threadid_motion = _threadmotioncapure.native_handle();
      _threadmotioncapure.detach();
    } else
      _threadmotioncapure.join();
  }

  // save data into sqlite database
  void save2database_t() {
    std::thread _thread(&threadloop::save2database, this);
    _threadid_database = _thread.native_handle();
    _thread.detach();
  }

  void closelooop() {
    if (FILEORNOT) {
      pthread_cancel(_threadid_pnsend);
      pthread_cancel(_threadid_gamepad);
      pthread_cancel(_threadid_motion);
      for (int i = 0; i != MAXCONNECTION; ++i) stopthread(i);
      pthread_cancel(_threadid_database);

      fclose(myfile);
    }
  }

  int get_connection_status() { return connection_status; }

  Vector6d getrealtimestate_first() { return _realtimevessel_first.State; }
  Vector6d getrealtimestate_second() { return _realtimevessel_second.State; }
  Vector6d getrealtimestate_third() { return _realtimevessel_third.State; }

  Vector6d getrealtime6dmotionmeasurement_first() {
    return _realtimevessel_first.Measurement;
  }
  Vector6d getrealtime6dmotionmeasurement_second() {
    return _realtimevessel_second.Measurement;
  }
  Vector6d getrealtime6dmotionmeasurement_third() {
    return _realtimevessel_third.Measurement;
  }
  Vector6d getrealtime6dmotion_first() {
    return _realtimevessel_first.Position;
  }
  Vector6d getrealtime6dmotion_second() {
    return _realtimevessel_second.Position;
  }
  Vector6d getrealtime6dmotion_third() {
    return _realtimevessel_third.Position;
  }
  Eigen::Vector3i getrealtimealphadeg_first() {
    return _realtimevessel_first.alpha_deg;
  }
  Eigen::Vector3i getrealtimealphadeg_second() {
    return _realtimevessel_second.alpha_deg;
  }
  Eigen::Vector3i getrealtimealphadeg_third() {
    return _realtimevessel_third.alpha_deg;
  }
  Eigen::Vector3i getrealtimerotation_first() {
    return _realtimevessel_first.rotation;
  }
  Eigen::Vector3i getrealtimerotation_second() {
    return _realtimevessel_second.rotation;
  }
  Eigen::Vector3i getrealtimerotation_third() {
    return _realtimevessel_third.rotation;
  }

 private:
  // thread pool
  typedef std::unordered_map<int, pthread_t> ThreadMap;
  ThreadMap _tmclients;          // the id array of thread for socket connection
  pthread_t _threadid_database;  // the id of thread for saving data
  pthread_t _threadid_motion;    // the id of thread for motion capture
  pthread_t _threadid_gamepad;   // the id of thread for gamepad
  pthread_t _threadid_pnsend;    // the id of thread for gamepad
  //
  int connection_status;
  databasecpp mydb;
  gamepadmonitor mygamepad;
  // mysql mysqli;
  motioncapture mymotioncapture;
  FILE *myfile;

  // constant parameters of the first vessel
  vessel_first _vessel_first{
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
      0.01,                                    // allowed_error_y;
      0.01,                                    // allowed_error_orientation;
      6.0,                                     // maxpositive_x_thrust(N)
      4.0,                                     // maxnegative_x_thrust(N)
      3.0,                                     // maxpositive_y_thrust(N)
      2.0,                                     // maxnegative_y_thrust(N)
      5.0,                                     // maxpositive_Mz_thrust(N*m)
      3.0,                                     // maxnegative_Mz_thrust(N*m)
      // 26.0,                                    // maxpositive_x_thrust(N)
      // 25.0,                                    // maxnegative_x_thrust(N)
      // 6,                                       // maxpositive_y_thrust(N)
      // 4,                                       // maxnegative_y_thrust(N)
      // 11,                                      //
      // maxpositive_Mz_thrust(N*m)
      // 7.6,                                     //
      // maxnegative_Mz_thrust(N*m)
      3,       // m
      3,       // n
      9,       // numvar
      3,       // num_constraints
      5.6e-7,  // Kbar_positive
      5e-7,    // Kbar_negative
      100,     // max_delta_rotation_bow
      4000,    // max_rotation_bow
      8.96,    // max_thrust_bow_positive
      8,       // max_thrust_bow_negative
      2e-5,    // K_left
      2e-5,    // K_right
      20,      // max_delta_rotation_bow
      1000,    // max_rotation_azimuth
      20,      // max_thrust_azimuth_left
      20,      // max_thrust_azimuth_right
      0.1277,  // max_delta_alpha_azimuth
      M_PI,    // max_alpha_azimuth_left
      0,       // min_alpha_azimuth_left
      0,       // max_alpha_azimuth_right
      -M_PI,   // min_alpha_azimuth_right
      1.9,     // bow_x
      0,       // bow_y
      -1.893,  // left_x
      -0.216,  // left_y
      -1.893,  // right_x
      0.216    // right_y
  };
  // constant parameters of the second vessel
  vessel_second _vessel_second{
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
      0.01,                                    // allowed_error_y;
      0.01,                                    // allowed_error_orientation;
      6.0,                                     // maxpositive_x_thrust(N)
      5.0,                                     // maxnegative_x_thrust(N)
      3,                                       // maxpositive_y_thrust(N)
      1,                                       // maxnegative_y_thrust(N)
      5,                                       // maxpositive_Mz_thrust(N*m)
      3,                                       // maxnegative_Mz_thrust(N*m)
      // 26.0,    // maxpositive_x_thrust(N)
      // 25.0,    // maxnegative_x_thrust(N)
      // 6,       // maxpositive_y_thrust(N)
      // 4,       // maxnegative_y_thrust(N)
      // 11,      // maxpositive_Mz_thrust(N*m)
      // 7.6,     // maxnegative_Mz_thrust(N*m)
      3,       // m
      3,       // n
      9,       // numvar
      3,       // num_constraints
      5.6e-7,  // Kbar_positive
      3e-7,    // Kbar_negative
      100,     // max_delta_rotation_bow
      4000,    // max_rotation_bow
      8.96,    // max_thrust_bow_positive
      5.0,     // max_thrust_bow_negative
      2e-5,    // K_left
      2e-5,    // K_right
      20,      // max_delta_rotation_bow
      1000,    // max_rotation_azimuth
      20,      // max_thrust_azimuth_left
      20,      // max_thrust_azimuth_right
      0.1277,  // max_delta_alpha_azimuth
      M_PI,    // max_alpha_azimuth_left
      0,       // min_alpha_azimuth_left
      0,       // max_alpha_azimuth_right
      -M_PI,   // min_alpha_azimuth_right
      1.9,     // bow_x
      0,       // bow_y
      -1.893,  // left_x
      -0.216,  // left_y
      -1.893,  // right_x
      0.216    // right_y
  };
  // constant parameters of the third vessel
  vessel_third _vessel_third{
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
      0.01,                                    // allowed_error_y;
      0.01,                                    // allowed_error_orientation;
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

  // controller of each vessel
  controller_first _controller_first;
  controller_second _controller_second;
  controller_third _controller_third;

  // stop one thread for client's socket
  void stopthread(int threadindex) {
    ThreadMap::const_iterator it = _tmclients.find(threadindex);
    if (it != _tmclients.end()) {
      pthread_cancel(it->second);
      _tmclients.erase(threadindex);
    }
  }

  // get the real time gamepad response
  void updategamepad() {
    while (1) {
      mygamepad.updategamepad();
    }
  }
  // get the real time motion response
  void updatemotion() {
    if (mymotioncapture.initializemotioncapture() == 0) {
      mymotioncapture.RequestPositionandVelocity(
          _realtimevessel_first, _realtimevessel_second, _realtimevessel_third);
    }

    // while (1) {
    //   mymotioncapture.RequestPositionandVelocity(
    //       _realtimevessel_first, _realtimevessel_second,
    //       _realtimevessel_third);
    // }
  }
  // save motion data to sqlite database
  void save2database() {
    while (1) {
      if (MAXCONNECTION > 0) {
        mydb.update_client_table(false, _realtimevessel_first);
      }
      if (MAXCONNECTION > 1) {
        mydb.update_client_table(false, _realtimevessel_second);
      }
      if (MAXCONNECTION > 2) {
      }
    }
  }

  // send and receive data from the first client (K class-I)
  void controller_first_pn() {
    while (1) {
      // real-time control and optimization for each client
      boost::posix_time::ptime t_start =
          boost::posix_time::second_clock::local_time();

      _controller_first.fullymanualcontroller(
          mygamepad.getGamepadXforce(), mygamepad.getGamepadYforce(),
          mygamepad.getGamepadZmoment(), _vessel_first, _realtimevessel_first,
          myfile);

      boost::posix_time::ptime t_end =
          boost::posix_time::second_clock::local_time();
      boost::posix_time::time_duration t_elapsed = t_end - t_start;
      long int mt_elapsed = t_elapsed.total_milliseconds();
      if (mt_elapsed > sample_mtime) {
        if (FILEORNOT) {
          myfile = fopen(logsavepath.c_str(), "a+");
          fprintf(myfile, "First: Take too long for QP!\n");
          fclose(myfile);
        } else
          perror("First: Take too long for QP");
      } else {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(sample_mtime - mt_elapsed));
      }
      realtimeprint_first();
    }
  }

  // send and receive data from the second client (K class-II)
  void controller_second_pn() {
    while (1) {
      // real-time control and optimization for each client
      boost::posix_time::ptime t_start =
          boost::posix_time::second_clock::local_time();

      _controller_second.fullymanualcontroller(
          mygamepad.getGamepadXforce(), mygamepad.getGamepadYforce(),
          mygamepad.getGamepadZmoment(), _vessel_second, _realtimevessel_second,
          myfile);

      boost::posix_time::ptime t_end =
          boost::posix_time::second_clock::local_time();
      boost::posix_time::time_duration t_elapsed = t_end - t_start;
      long int mt_elapsed = t_elapsed.total_milliseconds();
      if (mt_elapsed > sample_mtime) {
        if (FILEORNOT) {
          myfile = fopen(logsavepath.c_str(), "a+");
          fprintf(myfile, "Second: Take too long for QP!\n");
          fclose(myfile);
        } else
          perror("Second: Take too long for QP");
      } else {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(sample_mtime - mt_elapsed));
      }
      // realtimeprint_second();
    }
  }

  // send and receive data from the third client (X class)
  void controller_third_pn() {
    while (1) {
      // real-time control and optimization for each client
      boost::posix_time::ptime t_start =
          boost::posix_time::second_clock::local_time();

      _controller_third.fullymanualcontroller(
          mygamepad.getGamepadXforce(), mygamepad.getGamepadYforce(),
          mygamepad.getGamepadZmoment(), _vessel_third, _realtimevessel_third);

      boost::posix_time::ptime t_end =
          boost::posix_time::second_clock::local_time();
      boost::posix_time::time_duration t_elapsed = t_end - t_start;
      long int mt_elapsed = t_elapsed.total_milliseconds();
      if (mt_elapsed > sample_mtime) {
        if (FILEORNOT) {
          myfile = fopen(logsavepath.c_str(), "a+");
          fprintf(myfile, "Third: Take too long for QP!\n");
          fclose(myfile);
        } else
          perror("Third: Take too long for QP");
      } else {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(sample_mtime - mt_elapsed));
      }

      realtimeprint_third();
    }
  }

  void send2allclients_pn() {
    if (MAXCONNECTION == 1) {
      send2firstvessel(&_realtimevessel_first, myfile);
    } else if (MAXCONNECTION == 2) {
      send2firstvessel(&_realtimevessel_first, myfile);
      send2secondvessel(&_realtimevessel_second, myfile);

    } else if (MAXCONNECTION == 3) {
      std::this_thread::sleep_for(std::chrono::milliseconds(sample_mtime));
    }
  }

  void createtables() {
    for (int i = 0; i != MAXCONNECTION; ++i) {
      mydb.update_mastertable(i);
      mydb.create_client_table(i);
    }
  }
  void realtimeprint_first() {
    std::cout << "First: Desired force:" << std::endl
              << _realtimevessel_first.tau << std::endl;
    std::cout << "First:Estimated force:" << std::endl
              << _realtimevessel_first.BalphaU << std::endl;
    std::cout << "First:thruster angle:" << std::endl
              << _realtimevessel_first.alpha_deg << std::endl;
    std::cout << "First:thruster speed:" << std::endl
              << _realtimevessel_first.rotation << std::endl;
  }
  void realtimeprint_second() {
    std::cout << "Second: Desired force:" << std::endl
              << _realtimevessel_second.tau << std::endl;
    std::cout << "Second: Estimated force:" << std::endl
              << _realtimevessel_second.BalphaU << std::endl;
    std::cout << "Second: thruster angle:" << std::endl
              << _realtimevessel_second.alpha_deg << std::endl;
    std::cout << "Second: thruster speed:" << std::endl
              << _realtimevessel_second.rotation << std::endl;
  }
  void realtimeprint_third() {
    std::cout << "Desired force:" << std::endl
              << _realtimevessel_third.tau << std::endl;
    std::cout << "Estimated force:" << std::endl
              << _realtimevessel_third.BalphaU << std::endl;
    std::cout << "thruster angle:" << std::endl
              << _realtimevessel_third.alpha_deg << std::endl;
    std::cout << "thruster speed:" << std::endl
              << _realtimevessel_third.rotation << std::endl;
  }
};

#endif /* _THREADLOOP_HPP_*/
