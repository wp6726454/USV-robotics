/*
***********************************************************************
* thread_Loop.hpp: thread-based DP controller and network
* function to run the whole loop on server (including PN server,
* 6D motion capture, Kalman, PID, thruster allocation, joystick,
* save2sqlite, viewer).
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
#include "../controller/pidcontroller/include/setpoints.h"
#include "../joystick/include/gamepadmonitor.h"
#include "../motioncapture/include/motioncapture.h"
#include "../network/include/crccheck.h"
#include "../network/include/datapack.h"
#include "../network/include/pnserver_t.h"
#include "../sql/include/databasecpp.h"
#include "constants.h"

class threadloop {
 public:
  threadloop()
      : connection_status(0),
        mydb(defaultdbsavepath),
        index_controlmode_first(0),
        index_controlmode_second(0),
        index_controlmode_third(0),
        _controller_first(_vessel_first, _realtimevessel_first),
        _controller_second(_vessel_second, _realtimevessel_second),
        _controller_third(_vessel_third, _realtimevessel_third) {}
  ~threadloop() {}

  void initializelooop() {
    // open system file descriptors
    if (FILEORNOT) {
      // initialize pn server
      pnd_init();
    } else {
      // initialize socket server
      pnd_init();
    }
    mydb = databasecpp(dbsavepath);
    // sqlite to create master table
    mydb.create_mastertable();
  }
  // start socket server and wait for clients
  void start_connnection_t() {
    if (_SERV_CP_Startup() == 0) {
      _openpncontroller();
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
  // update setpoints
  void updatesetpoints_t() {
    std::thread _thread(&threadloop::updatesetpoints, this);
    _threadid_setpoints = _thread.native_handle();
    _thread.detach();
  }
  // reset all data, close controller, database and PN driver, and kill all
  // threads
  void closelooop() {
    if (FILEORNOT) {
      for (int i = 0; i != MAXCONNECTION; ++i) stopmosekthread(i);
      resetallvessels();  // set zero of each vessel
      std::this_thread::sleep_for(std::chrono::seconds(1));
      pthread_cancel(_threadid_pnsend);

      closemotioncapture();  // close qtm clients

      // // close all thread
      pthread_cancel(_threadid_setpoints);
      pthread_cancel(_threadid_gamepad);
      pthread_cancel(_threadid_motion);
      pthread_cancel(_threadid_database);

      _closepncontroller();  // close pn server
    }
  }

  // setup the control mode of all vessels
  void setcontrolmode(int _controlmode) {
    index_controlmode_first = _controlmode;
    index_controlmode_second = _controlmode;
    index_controlmode_third = _controlmode;
  }
  // setup the sqlite db path
  void setdbsavepath(const std::string &_projectname) {
    dbsavepath = "/home/skloe/Coding/CPP1X/USV/DPfloatover/QT/build/data/" +
                 _projectname + ".db";
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
  ThreadMap _tmclients;  // the id array of thread for socket connection
  pthread_t _threadid_setpoints;  // the id of thread for setpoints
  pthread_t _threadid_database;   // the id of thread for saving data
  pthread_t _threadid_motion;     // the id of thread for motion capture
  pthread_t _threadid_gamepad;    // the id of thread for gamepad
  pthread_t _threadid_pnsend;     // the id of thread for gamepad
  //
  int connection_status;
  databasecpp mydb;
  gamepadmonitor mygamepad;
  motioncapture mymotioncapture;
  FILE *myfile;
  FILE *myfile_first;
  FILE *myfile_second;
  FILE *myfile_third;
  std::string dbsavepath;
  // index for control mode
  // 0: manual control
  // 1: heading control
  // 2: automatic control
  int index_controlmode_first;
  int index_controlmode_second;
  int index_controlmode_third;
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
      0.0,                                     // maxpositive_x_thrust(N)
      0.0,                                     // maxnegative_x_thrust(N)
      0,                                       // maxpositive_y_thrust(N)
      0,                                       // maxnegative_y_thrust(N)
      0,                                       // maxpositive_Mz_thrust(N*m)
      0,                                       // maxnegative_Mz_thrust(N*m)
      3,                                       // m
      3,                                       // n
      9,                                       // numvar
      3,                                       // num_constraints
      5.6e-7,                                  // Kbar_positive
      2.0e-7,                                  // Kbar_negative
      100,                                     // max_delta_rotation_bow
      4000,                                    // max_rotation_bow
      8.96,                                    // max_thrust_bow_positive
      3.2,                                     // max_thrust_bow_negative
      2e-5,                                    // K_left
      2e-5,                                    // K_right
      20,                                      // max_delta_rotation_azimuth
      1000,                                    // max_rotation_azimuth
      50,                                      // min_rotation_azimuth
      20,                                      // max_thrust_azimuth_left
      20,                                      // max_thrust_azimuth_right
      0.05,                                    // min_thrust_azimuth_left
      0.05,                                    // min_thrust_azimuth_right
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
  // constant parameters of the second vessel
  vessel_second _vessel_second{
      {623, 0, 0, 0, 706, 444, 0, 444, 1298},  // mass
      {17, 0, 0, 0, 20, 0, 0, 0, 100},         // damping
      2.0,                                     // P_x
      1.5,                                     // P_y
      8.0,                                     // P_theta
      0.0,                                     // I_x
      0.0,                                     // I_y
      0.0,                                     // I_theta
      20.0,                                    // D_x
      5.0,                                     // D_y
      30.0,                                    // D_theta
      0.01,                                    // allowed_error_x
      0.01,                                    // allowed_error_y;
      0.02,                                    // allowed_error_orientation;
      6.0,                                     // maxpositive_x_thrust(N)
      5.0,                                     // maxnegative_x_thrust(N)
      3,                                       // maxpositive_y_thrust(N)
      1.5,                                     // maxnegative_y_thrust(N)
      5,                                       // maxpositive_Mz_thrust(N*m)
      3,                                       // maxnegative_Mz_thrust(N*m)
      3,                                       // m
      3,                                       // n
      9,                                       // numvar
      3,                                       // num_constraints
      5.5e-7,                                  // Kbar_positive
      1.0e-7,                                  // Kbar_negative
      100,                                     // max_delta_rotation_bow
      3000,                                    // max_rotation_bow
      4.95,                                    // max_thrust_bow_positive
      0.9,                                     // max_thrust_bow_negative
      2e-5,                                    // K_left
      2e-5,                                    // K_right
      20,                                      // max_delta_rotation_azimuth
      1000,                                    // max_rotation_azimuth
      20,                                      // min_rotation_azimuth
      20,                                      // max_thrust_azimuth_left
      20,                                      // max_thrust_azimuth_right
      0.008,                                   // min_thrust_azimuth_left
      0.008,                                   // min_thrust_azimuth_right
      0.1277,                                  // max_delta_alpha_azimuth
      M_PI,                                    // max_alpha_azimuth_left
      M_PI / 180,                              // min_alpha_azimuth_left
      -M_PI / 180,                             // max_alpha_azimuth_right
      -M_PI,                                   // min_alpha_azimuth_right
      1.9,                                     // bow_x
      0,                                       // bow_y
      -1.893,                                  // left_x
      -0.216,                                  // left_y
      -1.893,                                  // right_x
      0.216                                    // right_y
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
      Eigen::Vector3d::Zero(),  // setPoints
      Vector6d::Zero(),         // state
      Eigen::Vector3d::Zero(),  // tau
      Eigen::Vector3d::Zero(),  // BalphaU
      (Eigen::Vector3d() << -M_PI / 2, M_PI / 30, -M_PI / 30)
          .finished(),                                   // alpha
      Eigen::Vector3i::Zero(),                           // alpha_deg
      (Eigen::Vector3d() << 0.01, 0.2, 0.2).finished(),  // u
      Eigen::Vector3i::Zero()                            // rotation
  };
  // realtime parameters of the second vessel (K class-II)
  realtimevessel_second _realtimevessel_second{
      Vector6d::Zero(),         // measurement
      Vector6d::Zero(),         // position
      Eigen::Vector3d::Zero(),  // setPoints
      Vector6d::Zero(),         // state
      Eigen::Vector3d::Zero(),  // tau
      Eigen::Vector3d::Zero(),  // BalphaU
      (Eigen::Vector3d() << M_PI / 2, M_PI / 10, -M_PI / 4)
          .finished(),                                   // alpha
      Eigen::Vector3i::Zero(),                           // alpha_deg
      (Eigen::Vector3d() << 0.01, 0.2, 0.2).finished(),  // u
      Eigen::Vector3i::Zero()                            // rotation
  };
  // realtime parameters of the third vessel (X class)
  realtimevessel_third _realtimevessel_third{
      Vector6d::Zero(),         // measurement
      Vector6d::Zero(),         // position
      Eigen::Vector3d::Zero(),  // setPoints
      Vector6d::Zero(),         // state
      Eigen::Vector3d::Zero(),  // tau
      Eigen::Vector3d::Zero(),  // BalphaU
      (Eigen::Vector3d() << -M_PI / 2, M_PI / 180, -M_PI / 30)
          .finished(),                                    // alpha
      Eigen::Vector3i::Zero(),                            // alpha_deg
      (Eigen::Vector3d() << 0.001, 0.0, 0.0).finished(),  // u
      Eigen::Vector3i::Zero()                             // rotation
  };

  // setpoints
  fixedpointdata _fixedpointdata_second{6, 4, 0};

  setpoints mysetpoints;
  // controller of each vessel
  controller_first _controller_first;
  controller_second _controller_second;
  controller_third _controller_third;

  // stop one thread for client's mosek solver
  void stopmosekthread(int threadindex) {
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
    if (mymotioncapture.initializemotioncapture(myfile) == 0) {
      mymotioncapture.RequestPosition(_realtimevessel_first,
                                      _realtimevessel_second,
                                      _realtimevessel_third, myfile);
    }
  }
  void closemotioncapture() { mymotioncapture.stopRequest(myfile); }
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
    boost::posix_time::ptime t_start =
        boost::posix_time::second_clock::local_time();
    boost::posix_time::ptime t_end =
        boost::posix_time::second_clock::local_time();
    boost::posix_time::time_duration t_elapsed = t_end - t_start;
    long int mt_elapsed = 0;
    Eigen::Vector3d mysetpoint = _realtimevessel_first.State.head(3);
    while (1) {
      mysetpoint = _realtimevessel_first.State.head(3);
      // real-time control and optimization for each client
      t_start = boost::posix_time::second_clock::local_time();

      if (index_controlmode_first == 1) {
        _controller_first.headingcontrolleronestep(
            _realtimevessel_first, mysetpoint, mygamepad.getGamepadXforce(),
            mygamepad.getGamepadYforce(), myfile_first);
      } else if (index_controlmode_first == 2) {
        _controller_first.pidcontrolleronestep(_realtimevessel_first,
                                               mysetpoint, myfile_first);
      } else {
        _controller_first.fullymanualcontroller(
            mygamepad.getGamepadXforce(), mygamepad.getGamepadYforce(),
            mygamepad.getGamepadZmoment(), _realtimevessel_first, myfile_first);
      }
      t_end = boost::posix_time::second_clock::local_time();
      t_elapsed = t_end - t_start;
      mt_elapsed = t_elapsed.total_milliseconds();
      if (mt_elapsed > sample_mtime) {
        if (FILEORNOT) {
          myfile_first = fopen(logsavepath_first.c_str(), "a+");
          fprintf(myfile_first, "First: Take too long for QP!\n");
          fclose(myfile_first);
        } else
          perror("First: Take too long for QP");
        // continue;
      } else {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(sample_mtime - mt_elapsed));
      }
      // realtimeprint_first();
    }
  }

  // send and receive data from the second client (K class-II)
  void controller_second_pn() {
    boost::posix_time::ptime t_start =
        boost::posix_time::second_clock::local_time();
    boost::posix_time::ptime t_end =
        boost::posix_time::second_clock::local_time();
    boost::posix_time::time_duration t_elapsed = t_end - t_start;
    long int mt_elapsed = 0;
    while (1) {
      // real-time control and optimization for each client
      t_start = boost::posix_time::second_clock::local_time();

      if (index_controlmode_second == 1) {
        _controller_second.headingcontrolleronestep(
            _realtimevessel_second, mygamepad.getGamepadXforce(),
            mygamepad.getGamepadYforce(), myfile_second);
      } else if (index_controlmode_second == 2) {
        _controller_second.pidcontrolleronestep(_realtimevessel_second,
                                                myfile_second);
      } else {
        _controller_second.fullymanualcontroller(
            mygamepad.getGamepadXforce(), mygamepad.getGamepadYforce(),
            mygamepad.getGamepadZmoment(), _realtimevessel_second,
            myfile_second);
      }
      t_end = boost::posix_time::second_clock::local_time();
      t_elapsed = t_end - t_start;
      mt_elapsed = t_elapsed.total_milliseconds();
      if (mt_elapsed > sample_mtime) {
        if (FILEORNOT) {
          myfile_second = fopen(logsavepath_second.c_str(), "a+");
          fprintf(myfile_second, "Second: Take too long for QP!\n");
          fclose(myfile_second);
        } else
          perror("Second: Take too long for QP");
        // continue;
      } else {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(sample_mtime - mt_elapsed));
      }
      realtimeprint_second();
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
          mygamepad.getGamepadZmoment(), _realtimevessel_third);

      boost::posix_time::ptime t_end =
          boost::posix_time::second_clock::local_time();
      boost::posix_time::time_duration t_elapsed = t_end - t_start;
      long int mt_elapsed = t_elapsed.total_milliseconds();
      if (mt_elapsed > sample_mtime) {
        if (FILEORNOT) {
          myfile_third = fopen(logsavepath_third.c_str(), "a+");
          fprintf(myfile_third, "Third: Take too long for QP!\n");
          fclose(myfile_third);
        } else
          perror("Third: Take too long for QP");
      } else {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(sample_mtime - mt_elapsed));
      }

      realtimeprint_third();
    }
  }
  // function for send data to all clients using PN
  void send2allclients_pn() {
    if (MAXCONNECTION == 1) {
      send2firstvessel(&_realtimevessel_first, myfile);
    } else if (MAXCONNECTION == 2) {
      send2bothvessel(&_realtimevessel_first, &_realtimevessel_second, myfile);
    } else if (MAXCONNECTION == 3) {
      std::this_thread::sleep_for(std::chrono::milliseconds(sample_mtime));
    }
  }
  // create sqlite database
  void createtables() {
    for (int i = 0; i != MAXCONNECTION; ++i) {
      mydb.update_mastertable(i);
      mydb.create_client_table(i);
    }
  }
  // update setpoints of each vessel
  void updatesetpoints() {
    mysetpoints.gofixedpoint_second(_realtimevessel_second,
                                    _fixedpointdata_second);
  }

  // reset realtime data of each vessel
  void resetallvessels() {
    // reset the data of the first vessel
    _realtimevessel_first.Measurement.setZero();
    _realtimevessel_first.Position.setZero();
    _realtimevessel_first.State.setZero();
    _realtimevessel_first.tau.setZero();
    _realtimevessel_first.BalphaU.setZero();
    _realtimevessel_first.alpha.setZero();
    _realtimevessel_first.alpha_deg.setZero();
    _realtimevessel_first.u.setZero();
    _realtimevessel_first.rotation.setZero();
    // reset the data of the second vessel
    _realtimevessel_second.Measurement.setZero();
    _realtimevessel_second.Position.setZero();
    _realtimevessel_second.State.setZero();
    _realtimevessel_second.tau.setZero();
    _realtimevessel_second.BalphaU.setZero();
    _realtimevessel_second.alpha.setZero();
    _realtimevessel_second.alpha_deg.setZero();
    _realtimevessel_second.u.setZero();
    _realtimevessel_second.rotation.setZero();
    // reset the data of the third vessel
    _realtimevessel_third.Measurement.setZero();
    _realtimevessel_third.Position.setZero();
    _realtimevessel_third.State.setZero();
    _realtimevessel_third.tau.setZero();
    _realtimevessel_third.BalphaU.setZero();
    _realtimevessel_third.alpha.setZero();
    _realtimevessel_third.alpha_deg.setZero();
    _realtimevessel_third.u.setZero();
    _realtimevessel_third.rotation.setZero();
  }  // setZero for realtime data of each vessel

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
